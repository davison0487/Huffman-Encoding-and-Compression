#include <iostream>
#include <string>

#include <gtest/gtest.h>
#include "BitInputStream.hpp"

using namespace std;
using namespace testing;

TEST(BitInputStreamTests, SIMPLE_TEST) {
    string byteStr = "10000000";
    char byte = (char)stoi(byteStr, nullptr, 2);

    stringstream ss;
    ss.write(&byte, 1);
    BitInputStream bis(ss, 1);

    ASSERT_EQ(1, bis.readBit());
    ASSERT_EQ(0, bis.readBit());
}