#include "BitOutputStream.hpp"

BitOutputStream::~BitOutputStream() { delete buf; }
/**
 * TODO: Write the part of the buffer that was written by the user to the output
 * stream, and then clear the buffer to allow further use. You may use fill() to
 * zero-fill the buffer.
 *
 * Note: don’t flush the ostream here, as it is an extremely slow operation that
 * may cause a timeout.
 */
void BitOutputStream::flush() {
    int numChars = nbits / 8;
    if ((nbits % 8) != 0) ++numChars;
    out.write(buf, numChars);
    nbits = 0;
    std::fill(buf, buf + bufSize, '\0');
}

/**
 * TODO: Write the least significant bit of the given int to the bit buffer.
 * Flushes the buffer first if it is full (which means all the bits in the
 * buffer have already been set). You may assume the given int is either 0 or 1.
 */
void BitOutputStream::writeBit(unsigned int i) {
    if (nbits == (bufSize * 8)) flush();
    int n = nbits / 8;
    int r = 7 - (nbits % 8);
    buf[n] |= (i << r);
    ++nbits;
}
