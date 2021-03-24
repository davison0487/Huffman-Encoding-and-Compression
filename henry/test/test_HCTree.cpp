#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

#include "BitInputStream.hpp"
#include "BitOutputStream.hpp"
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
        freqs['+'] = 16;
        freqs['O'] = 23;
        freqs['A'] = 30;
        freqs['C'] = 40;
        freqs['E'] = 41;
        freqs['S'] = 44;
        freqs['N'] = 52;
        freqs['R'] = 68;
        freqs['T'] = 75;
        freqs['H'] = 82;
        freqs['I'] = 87;
        tree.build(freqs);
    }
};

TEST_F(SimpleHCTreeFixture, TEST_ENCODE) {
    ostringstream os;
    tree.encode('T', os);
    ASSERT_EQ(os.str(), "100");
}

TEST_F(SimpleHCTreeFixture, TEST_DECODE) {
    istringstream is("100");
    ASSERT_EQ(tree.decode(is), 'T');
}

TEST_F(SimpleHCTreeFixture, TEST_TRUEENCODE) {
    ostringstream os;
    tree.encode('T', os);
    ASSERT_EQ(os.str(), "100");
}
