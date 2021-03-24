/**
 * A converter between chars to their binary number representation.
 */
#include <string.h>

#include <fstream>
#include <vector>
#include "BitInputStream.hpp"
#include "BitOutputStream.hpp"

using namespace std;

/*
 * Convert a sequence of bits in inFile to a sequence of chars in outFile. Each
 * 8 bits will correspond to 1 char. If the number of bits n is not divisible by
 * 8, then the last n % 8 bits will be combined with padding 0s to get the
 * corresponding char
 */
void bitToChar(string inFileName, string outFileName, unsigned int bufSize) {
    ifstream inFile;
    ofstream outFile;
    inFile.open(inFileName, ios::binary);
    outFile.open(outFileName);

    BitOutputStream out(outFile, bufSize);
    while (1) {
        byte nextChar = inFile.get();
        if (inFile.eof()) break;
        if (nextChar == '0') {
            out.writeBit(0);
        } else {
            out.writeBit(1);
        }
    }
    out.flush();
    inFile.close();
    outFile.close();
}

/*
 * Convert a sequence of chars in inFile to a sequence of bits in outFile. Each
 * char will correspond to 8 bits.
 */
void charToBit(string inFileName, string outFileName, unsigned int bufSize) {
    ifstream inFile;
    ofstream outFile;
    inFile.open(inFileName, ios::binary);
    outFile.open(outFileName);

    BitInputStream in(inFile, bufSize);

    // use vector buffer
    vector<unsigned char> bitsBuf;
    while (1) {
        unsigned int nextBit = in.readBit();
        if (in.eof()) break;
        unsigned char bit = (nextBit == 0) ? '0' : '1';
        bitsBuf.push_back(bit);
    }

    // write out the buffer
    outFile.write((const char*)&bitsBuf[0], bitsBuf.size());
    inFile.close();
    outFile.close();
}

/* Main function that runs the program */
int main(int argc, char* argv[]) {
    const int NUM_ARG = 5;
    const int FLAG_IDX = 1;
    const int BUF_IDX = 2;
    const int IN_IDX = 3;
    const int OUT_IDX = 4;

    if (argc != NUM_ARG) return 1;
    unsigned int bufSize = stoi(argv[BUF_IDX]);
    if (strcmp(argv[FLAG_IDX], "-b") == 0) {
        charToBit(argv[IN_IDX], argv[OUT_IDX], bufSize);
    } else if (strcmp(argv[FLAG_IDX], "-c") == 0) {
        bitToChar(argv[IN_IDX], argv[OUT_IDX], bufSize);
    }
    return 0;
}