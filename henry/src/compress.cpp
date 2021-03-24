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

/* TODO: add pseudo compression with ascii encoding and naive header
 * (checkpoint) */
void pseudoCompression(const string& inFileName, const string& outFileName) {
    // check if the input file is valid
    if (FileUtils::isValidFile(inFileName)) {
        ifstream inFile;
        ofstream outFile;
        inFile.open(inFileName);
        outFile.open(outFileName);

        // loop getting single characters and store its frequency
        vector<unsigned int> freq(256, 0);
        while (inFile.peek() != EOF) {
            byte c = inFile.get();  // inFIle.get() will return type int
            freq[c]++;
        }

        // reset the file pointer to the start
        inFile.clear();
        inFile.seekg(0, std::ios::beg);

        // build the Huffman tree and get the encoded bits
        HCTree tree;
        tree.build(freq);
        string encodedStr = "";
        char c;
        while (inFile.get(c)) {
            ostringstream os;
            tree.encode(c, os);
            encodedStr += os.str();
        }

        // write each char's frequency into header
        // and the encoded bits int oline 257
        for (unsigned int i = 0; i < freq.size(); ++i) {
            outFile << freq[i] << "\n";
        }
        outFile << encodedStr;

        inFile.close();
        outFile.close();
    }
}

/* TODO: True compression with bitwise i/o and small header (final) */
// helper function for true compression
void PostTraversal(BitOutputStream& out, HCNode* node) {
    if (node != nullptr) {
        // do a post order traversal
        PostTraversal(out, node->c0);
        PostTraversal(out, node->c1);

        // leaf node
        if (node->c0 == nullptr && node->c1 == nullptr) {
            // std::cout << 1;
            out.writeBit(1);
            // std::cout << node->symbol;
            out.writeSymbol(node->symbol);
        } else {
            // std::cout << 0;
            out.writeBit(0);
        }
    }
    return;
}

/* TODO: True compression with bitwise i/o and small header (final) */
void trueCompression(const string& inFileName, const string& outFileName) {
    HCTree tree;
    vector<unsigned int> freqs(256, 0);

    ifstream inFile;
    inFile.open(inFileName, ifstream::in);
    ofstream outFile;
    outFile.open(outFileName, ofstream::out);

    ostringstream os;  // read the encode result
    BitOutputStream outstream(outFile, 4000);

    unsigned int numofSymbols = 0;

    // read the file char by char
    // update the freq chart
    char c;
    string result;
    if (inFile.is_open()) {
        while (inFile.peek() != EOF) {
            freqs[inFile.get()]++;
            numofSymbols++;
        }
        inFile.clear();
        inFile.seekg(0, ios::beg);
    }

    // build the tree with the chart
    tree.build(freqs);
    HCNode* root = tree.Root();

    if (root != nullptr) {
        std::cout << "num of symbols " << numofSymbols << std::endl;
        outstream.writeNum(numofSymbols);
        // go through the tree and output to a new file
        PostTraversal(outstream, root);
        // use "0" as the end of file
        outstream.writeBit(0);
        // std::cout << 0 << std::endl;
    }

    if (inFile.is_open()) {
        while (inFile.peek() != EOF) {
            char c = inFile.get();
            tree.encode(c, outstream);
        }
    }

    outstream.flush();
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
        std::cout << "preudo compressing file..." << std::endl;
        pseudoCompression(inFileName, outFileName);
    } else {
        trueCompression(inFileName, outFileName);
    }

    return 0;
}