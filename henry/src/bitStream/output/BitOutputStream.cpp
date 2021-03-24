#include "BitOutputStream.hpp"

#include <iostream>
#define BUF_PER_SIZE 8
#define INT_SIZE 32

// class destructor
BitOutputStream::~BitOutputStream() { delete[] buf; }

/**
 * TODO: Write the part of the buffer that was written by the user to the output
 * stream, and then clear the buffer to allow further use. You may use fill() to
 * zero-fill the buffer.
 *
 * Note: don’t flush the ostream here, as it is an extremely slow operation that
 * may cause a timeout.
 */
void BitOutputStream::flush() {
    // determine number of bytes to flush
    unsigned int flushSize = (nbits % 8 == 0) ? (nbits / 8) : (nbits / 8 + 1);
    out.write(buf, flushSize);

    // clear buffer and bitcounter
    nbits = 0;
    fill(buf, buf + bufSize, 0);
}

/**
 * TODO: Write the least significant bit of the given int to the bit buffer.
 * Flushes the buffer first if it is full (which means all the bits in the
 * buffer have already been set). You may assume the given int is either 0 or 1.
 */
void BitOutputStream::writeBit(unsigned int i) {
    // flush the buffer if it is full
    if (nbits == bufSize * 8) flush();

    // write the LSB of i into the buffer
    int byteIndex = nbits / 8;
    int bitIndex = nbits % 8;
    buf[byteIndex] |= (i << (7 - bitIndex));
    nbits++;
}

void BitOutputStream::writeSymbol(unsigned int j) {
    // flush the buffer if it is full
    if (nbits == bufSize * 8) flush();

    char byte = (char)j;
    for (int i = BUF_PER_SIZE - 1; i >= 0; i--) {
        // get the MSB
        int bit = (byte >> i) & 1;
        writeBit(bit);
    }
}

void BitOutputStream::convertIntegerToBits(unsigned int number, int count) {
    if (count < 32) {
        count++;
        convertIntegerToBits(number >> 1, count);
        bool curr = number & 1;
        // cout << curr;
        writeBit(curr);
    }
    return;
}

void BitOutputStream::writeNum(unsigned int i) {
    // flush the buffer if it is full
    if (nbits == bufSize * 8) flush();

    // for (int index = INT_SIZE - BUF_PER_SIZE; index >= 0;
    //      index -= BUF_PER_SIZE) {
    //     // read and change to char
    //     int byte = i >> index;
    //     char character = (char)byte;
    //     std::cout << character;
    //     // write and flush
    //     out.put(character);
    //     out.flush();
    // }
    int count = 0;
    convertIntegerToBits(i, count);
}