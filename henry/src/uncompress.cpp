/**
 * TODO: file header
 *
 * Author:
 */
#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "FileUtils.hpp"
#include "HCNode.hpp"
#include "HCTree.hpp"

/* TODO: Pseudo decompression with ascii encoding and naive header (checkpoint)
 */
void pseudoDecompression(const string& inFileName, const string& outFileName) {
    HCTree tree;
    ifstream inFile(inFileName);
    ofstream outFile(outFileName);
    vector<unsigned int> freqs(256, 0);
    ostringstream os;  // read the encode result

    string line;
    string result;
    if (inFile.is_open()) {
        for (int i = 0; i < freqs.size(); ++i) {
            getline(inFile, line);
            int tmp = stoi(line);
            freqs[i] = tmp;
        }
        getline(inFile, line);
    }
    inFile.close();
    tree.build(freqs);
    istringstream is(line);
    while (!(is >> std::ws).eof()) {
        result += tree.decode(is);
    }
    if (outFile.is_open()) {
        outFile << result;
    }
    outFile.close();
}

/* TODO: True decompression with bitwise i/o and small header (final) */
void trueDecompression(const string& inFileName, const string& outFileName) {
    HCTree tree;
    ifstream inFile;
    inFile.open(inFileName, ifstream::in);
    ofstream outFile;
    outFile.open(outFileName, ofstream::out);
    vector<unsigned int> freqs(256, 0);
    ostringstream os;  // read the encode result

    BitInputStream instream(inFile, 4000);
    BitOutputStream outstream(outFile, 4000);
    unsigned int numofSymbols = instream.readNum();
    std::cout << numofSymbols << std::endl;

    tree.build(instream);
    HCNode* root = tree.Root();

    if (root != nullptr) {
        int readCount = 1;
        while (readCount <= numofSymbols) {
            int symbol = tree.decode(instream);
            if (symbol != -1) {
                outstream.writeSymbol(symbol);
                readCount++;
            }
        }
    }
    outstream.flush();

    inFile.close();
    outFile.close();
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
        std::cout << "pseudo decompressing file..." << std::endl;
        pseudoDecompression(inFileName, outFileName);
    } else {
        trueDecompression(inFileName, outFileName);
    }

    return 0;
}
