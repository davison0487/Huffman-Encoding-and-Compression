/**
 * TODO: file header
 *
 * Author:
 */
#include <algorithm>
#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "FileUtils.hpp"
#include "HCNode.hpp"
#include "HCTree.hpp"

/* TODO: Pseudo decompression with ascii encoding and naive header (checkpoint)
 */
void pseudoDecompression(const string& inFileName, const string& outFileName) {
    ifstream ifile;
    ifile.open(inFileName, ios::in);

    vector<unsigned int> freq(256, 0);
    for (int i = 0; i < 256; ++i) {
        string temp;
        getline(ifile, temp);
        int curr = stoi(temp);
        freq[i] = curr;
    }

    HCTree tree;
    tree.build(freq);

    ofstream ofile;
    ofile.open(outFileName, ios::out);

    while (ifile.peek() != EOF) ofile << tree.decode(ifile);
}

/* TODO: True decompression with bitwise i/o and small header (final) */
void trueDecompression(const string& inFileName, const string& outFileName) {
    unsigned int bufSize = 4000;
    ifstream ifile;
    ifile.open(inFileName, ios::binary);
    HCTree tree;
    ofstream ofile;
    ofile.open(outFileName);
    BitInputStream bi(ifile, bufSize);
    vector<unsigned int> freq(256, 0);

    /* TODO
    vector<unsigned int> freq(256, 0);
    int total = 0;
    for (int i = 0; i < 256; ++i) {
        string temp;
        getline(ifile, temp);
        int curr = stoi(temp);
        freq[i] = curr;
        total = total + curr;
    }*/
    int numBits = 0;
    int numNon = 0;
    int total = 0;
    for (int i = 0; i < 5; ++i) {
        numBits <<= 1;
        numBits |= bi.readBit();
    }
    for (int i = 0; i < 8; ++i) {
        numNon <<= 1;
        numNon |= bi.readBit();
    }
    for (int i = 0; i < numNon; ++i) {
        unsigned int ch = 0;
        unsigned int num = 0;
        for (int j = 0; j < 8; ++j) {
            ch <<= 1;
            ch |= bi.readBit();
        }
        for (int j = 0; j < numBits; ++j) {
            num <<= 1;
            num |= bi.readBit();
        }
        freq[ch] = num;
        total += num;
    }

    tree.build(freq);
    int count = 0;
    while (true) {
        unsigned char c = tree.decode(bi);
        if (bi.eof() == true || count == total) break;
        ofile << c;
        ++count;
    }
}

/* Main program that runs the decompression */
int main(int argc, char* argv[]) {
    cxxopts::Options options(argv[0],
                             "Uncompresses files using Huffman Encoding");
    options.positional_help(
        "./path_to_compressed_input_file ./path_to_output_file");

    bool isAscii = false;
    string inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "ascii", "Read input in ascii mode instead of bit stream",
        cxxopts::value<bool>(isAscii))("input", "",
                                       cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h,help", "Print help and exit.");

    options.parse_positional({"input", "output"});
    auto userOptions = options.parse(argc, argv);

    if (userOptions.count("help") || !FileUtils::isValidFile(inFileName) ||
        outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        exit(0);
    }

    // if compressed file is empty, output empty file
    if (FileUtils::isEmptyFile(inFileName)) {
        ofstream outFile;
        outFile.open(outFileName, ios::out);
        outFile.close();
        exit(0);
    }

    if (isAscii) {
        pseudoDecompression(inFileName, outFileName);
    } else {
        trueDecompression(inFileName, outFileName);
    }

    return 0;
}
