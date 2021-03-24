/**
 * TODO: file header
 *
 * Author:
 */
#include <cxxopts.hpp>
#include <fstream>
#include <iostream>
#include <set>

#include "FileUtils.hpp"
#include "HCNode.hpp"
#include "HCTree.hpp"

string int2bin(int a, int const nums);
string ch2bin(unsigned char ch);

string int2bin(int a, int const nums) {
    string bin = "";
    while (a > 0) {
        bin = to_string(a % 2) + bin;
        a /= 2;
    }
    while (bin.size() != nums) bin = "0" + bin;
    return bin;
};

string ch2bin(unsigned char ch) {
    string bin = "";
    unsigned int a = ch;
    while (a > 0) {
        bin = to_string(a % 2) + bin;
        a /= 2;
    }
    while (bin.size() != 8) bin = "0" + bin;
    return bin;
};

/* TODO: add pseudo compression with ascii encoding and naive header
 * (checkpoint) */
void pseudoCompression(const string& inFileName, const string& outFileName) {
    ifstream ifile;
    ifile.open(inFileName);

    vector<unsigned int> freq(256, 0);
    while (ifile.peek() != EOF) freq[ifile.get()]++;
    HCTree tree;
    tree.build(freq);
    ifile.close();

    ofstream ofile;
    ofile.open(outFileName, ios::binary);
    for (auto i : freq) ofile << i << "\r\n";

    ifile.open(inFileName, ios::binary);
    while (ifile.peek() != EOF) {
        byte c = ifile.get();
        tree.encode(c, ofile);
    }
}

/* TODO: True compression with bitwise i/o and small header (final) */
void trueCompression(const string& inFileName, const string& outFileName) {
    unsigned int bufSize = 4000;
    ifstream ifile;
    ifile.open(inFileName);
    ofstream ofile;
    ofile.open(outFileName, ios::binary);
    BitOutputStream bo(ofile, bufSize);
    HCTree tree;

    vector<unsigned int> freq(256, 0);
    set<unsigned char> list;

    /* TODO: header design*/
    // for (auto i : freq) ofile << i << "\r\n";
    while (ifile.peek() != EOF) {
        unsigned char ch = ifile.get();
        freq[ch]++;
        list.emplace(ch);
    }
    ifile.close();

    int max_num = 0;
    int max_num_bit = 0;
    for (auto i : freq) max_num = (max_num > i) ? max_num : i;
    while (max_num) {
        max_num /= 2;
        ++max_num_bit;
    }

    string numBits = int2bin(max_num_bit, 5);
    for (int i = 0; i < numBits.size(); ++i) {
        bo.writeBit(numBits[i] - '0');
    }
    string non_zero = int2bin(list.size(), 8);
    for (int i = 0; i < non_zero.size(); ++i) {
        bo.writeBit(non_zero[i] - '0');
    }
    for (auto curr : list) {
        string temp = ch2bin(curr);
        temp += int2bin(freq[curr], max_num_bit);
        for (int i = 0; i < temp.size(); ++i) {
            bo.writeBit(temp[i] - '0');
        }
    }

    tree.build(freq);

    ifile.open(inFileName, ios::in);
    while (ifile.peek() != EOF) {
        byte c = ifile.get();
        tree.encode(c, bo);
    }
    bo.flush();
}

/* Main program that runs the compression */
int main(int argc, char* argv[]) {
    cxxopts::Options options(argv[0],
                             "Compresses files using Huffman Encoding");
    options.positional_help("./path_to_input_file ./path_to_output_file");

    bool isAsciiOutput = false;
    string inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "ascii", "Write output in ascii mode instead of bit stream",
        cxxopts::value<bool>(isAsciiOutput))(
        "input", "", cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h,help", "Print help and exit");

    options.parse_positional({"input", "output"});
    auto userOptions = options.parse(argc, argv);

    if (userOptions.count("help") || !FileUtils::isValidFile(inFileName) ||
        outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        return 0;
    }

    // if original file is empty, output empty file
    if (FileUtils::isEmptyFile(inFileName)) {
        ofstream outFile;
        outFile.open(outFileName, ios::out);
        outFile.close();
        return 0;
    }

    if (isAsciiOutput) {
        pseudoCompression(inFileName, outFileName);
    } else {
        trueCompression(inFileName, outFileName);
    }

    return 0;
}