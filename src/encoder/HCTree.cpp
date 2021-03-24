#include "HCTree.hpp"

#include <stack>

void destructor_helper(HCNode* curr);

/* TODO: Delete all objects on the heap to avoid memory leaks. */
HCTree::~HCTree() {
    destructor_helper(root);
    root = nullptr;
}

void destructor_helper(HCNode* curr) {
    if (curr->c0 != 0) {
        destructor_helper(curr->c0);
    }
    if (curr->c1 != 0) {
        destructor_helper(curr->c1);
    }
    delete curr;
    curr = nullptr;
};

/**
 * TODO: Build the HCTree from the given frequency vector. You can assume the
 * vector must have size 256 and each value at index i represents the frequency
 * of char with ASCII value i. Only non-zero frequency symbols should be used to
 * build the tree. The leaves vector must be updated so that it can be used in
 * encode() to improve performance.
 *
 * When building the HCTree, you should use the following tie-breaking rules to
 * match the output from reference solution in checkpoint:
 *
 *    1. HCNode with lower count should have higher priority. If count is the
 * same, then HCNode with a larger ascii value symbol should have higher
 * priority. (This should be already defined properly in the comparator in
 * HCNode.hpp)
 *    2. When popping two highest priority nodes from PQ, the higher priority
 * node will be the ‘c0’ child of the new parent HCNode.
 *    3. The symbol of any parent node should be taken from its 'c0' child.
 */
void HCTree::build(const vector<unsigned int>& freqs) {
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;
    for (int i = 0; i < freqs.size(); ++i) {
        HCNode* curr = 0;
        if (freqs[i] != 0) {
            curr = new HCNode(freqs[i], byte(i));
            pq.push(curr);
        }
        leaves[i] = curr;
        curr = nullptr;
    }
    while (pq.size() != 0) {
        if (pq.size() == 1) {
            root = pq.top();
            break;
        }
        HCNode* left = pq.top();
        pq.pop();
        HCNode* right = pq.top();
        pq.pop();
        HCNode* parent = new HCNode(left->count + right->count, left->symbol,
                                    left, right, 0);
        left->p = parent;
        right->p = parent;
        pq.push(parent);
        left = right = parent = nullptr;
    }
}

/**
 * TODO: Write the encoding bits of the given symbol to the ostream. You should
 * write each encoding bit as ascii char either '0' or '1' to the ostream. You
 * must not perform a comprehensive search to find the encoding bits of the
 * given symbol, and you should use the leaves vector instead to achieve
 * efficient encoding. For this function to work, build() must be called before
 * to create the HCTree.
 */
void HCTree::encode(byte symbol, BitOutputStream& out) const {
    stack<unsigned int> code;
    HCNode* curr = leaves[symbol];
    if (curr == root) code.push(0);
    while (curr != root) {
        HCNode* parent = curr->p;
        if (curr == parent->c0)
            code.push(0);
        else
            code.push(1);
        curr = parent;
        parent = nullptr;
    }
    while (!code.empty()) {
        out.writeBit(code.top());
        code.pop();
    }
}

/**
 * TODO: Write the encoding bits of the given symbol to ostream. You should
 * write each encoding bit as ascii char either '0' or '1' to the ostream. You
 * must not perform a comprehensive search to find the encoding bits of the
 * given symbol, and you should use the leaves vector instead to achieve
 * efficient encoding. For this function to work, build() must have been called
 * beforehand to create the HCTree.
 */
void HCTree::encode(byte symbol, ostream& out) const {
    string code = "";
    HCNode* curr = leaves[symbol];
    if (curr == root) code = "0";
    while (curr != root) {
        HCNode* parent = curr->p;
        if (curr == parent->c0)
            code = "0" + code;
        else
            code = "1" + code;
        curr = parent;
        parent = nullptr;
    }
    out << code;
}

/**
 * TODO: Decode the sequence of bits (represented as a char of either '0' or
 * '1') from the istream to return the coded symbol. For this function to work,
 * build() must have been called beforehand to create the HCTree.
 */
byte HCTree::decode(BitInputStream& in) const {
    HCNode* curr = root;
    while (curr->c0 != 0 && curr->c1 != 0) {
        int code = in.readBit();
        if (in.eof() == true)
            return 0;
        else if (code == 0)
            curr = curr->c0;
        else
            curr = curr->c1;
    }
    if (curr == root) in.readBit();
    if (in.eof() == true) return 0;
    return curr->symbol;
}

/**
 * TODO: Decode the sequence of bits (represented as char of either '0' or '1')
 * from istream to return the coded symbol. For this function to work, build()
 * must have been called beforehand to create the HCTree.
 */
byte HCTree::decode(istream& in) const {
    HCNode* curr = root;
    while (curr->c0 != 0 && curr->c1 != 0) {
        if (in.get() == '0')
            curr = curr->c0;
        else
            curr = curr->c1;
    }
    if (curr == root) in.get();
    return curr->symbol;
}
