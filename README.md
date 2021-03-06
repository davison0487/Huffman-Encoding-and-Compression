# Huffman Encoding and Compression

In this project, we will be implementing a Huffman Coding Tree and use it to perform pseudo compression without using bitwise i/o. The second part is to implement bitwise i/o with arbitrary buffer size to allow true compression with efficient header.

## Pseudo Compression
- Implement a Huffman Coding Tree to use as the compression algorithm.
- Implement a pseudo compress program that “compresses” the given file to an encoded file.
- Implement a pseudo decompress program that “decompresses” the file generated by the compress program to get the original file back.

## True Compression with Bitwise i/o and Header Design
- CMakeLists.txt in the bitstream folder
- Implement bitwise i/o with a buffer to allow for true compression
- Test bitstream using the completed executable bitconverter
- Implement encode and decode with bitwise i/o in HCTree to perform true compression
- Design an efficient header so that it takes as little space as possible in the compressed file
