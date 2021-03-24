#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "HCTree.hpp"

using namespace std;
using namespace testing;

class SimpleHCTreeFixture : public ::testing::Test {
  protected:
    HCTree tree;

  public:
    SimpleHCTreeFixture() {
        // initialization code here
        vector<unsigned int> freqs(256);
        freqs['a'] = 3;
        freqs['b'] = 3;
        tree.build(freqs);
    }
};

TEST_F(SimpleHCTreeFixture, TEST_ENCODE) {
    ostringstream os;
    tree.encode('a', os);
    ASSERT_EQ(os.str(), "1");
}

TEST_F(SimpleHCTreeFixture, TEST_DECODE) {
    istringstream is("0");
    ASSERT_EQ(tree.decode(is), 'b');
}

TEST_F(SimpleHCTreeFixture, TEST_BINARY_ENCODE) {
    ostringstream os;
    BitOutputStream bo(os, 100);
    tree.encode('a', bo);
    bo.flush();
    unsigned char temp = os.str()[0];
    ASSERT_EQ(temp, 128);
}

TEST_F(SimpleHCTreeFixture, TEST_BINARY_DECODE) {
    istringstream is("0");
    BitInputStream bi(is, 100);
    ASSERT_EQ(tree.decode(bi), 'b');
}