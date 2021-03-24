#include "HCTree.hpp"

#include <bitset>
#include <queue>
#include <stack>

HCNode* HCTree::Root() const { return root; }

/* TODO: Delete all objects on the heap to avoid memory leaks. */
void traverse(HCNode* ptr) {
    if (ptr == nullptr) return;
    traverse(ptr->c0);
    traverse(ptr->c1);
    delete ptr;
}

HCTree::~HCTree() { traverse(root); }

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

    // construct tree node and store into priority queue / leaves
    for (unsigned int i = 0; i < freqs.size(); ++i) {
        if (freqs[i] != 0) {
            HCNode* node = new HCNode(freqs[i], i);
            leaves[i] = node;
            pq.push(node);
        }
    }

    // construct the tree
    while (pq.size() > 1) {
        // select two smallest-count node and create new node
        HCNode* node1 = pq.top();
        pq.pop();
        HCNode* node2 = pq.top();
        pq.pop();

        unsigned int totalFreq = node1->count + node2->count;
        HCNode* newNode = new HCNode(totalFreq, node1->symbol, node1, node2);
        node1->p = newNode;
        node2->p = newNode;
        pq.push(newNode);
    }

    // set tree root to the last element
    root = pq.top();
    pq.pop();
}

// overloaded function of build()
void HCTree::build(BitInputStream& in) {
    // to keep track whether the next thing we are reading is a bit
    std::stack<HCNode*> myStack;
    bool next_bit = true;

    while (true) {
        // std::cout << "stack size << " << myStack.size() << std::endl;
        if (next_bit == true) {
            int bitRead = in.readBit();
            if (bitRead == -1) break;

            // when meet the 1, next one is leaf node
            if (bitRead == 1) {
                next_bit = false;
            } else {
                // 2 children, generate a parent
                if (myStack.size() >= 2) {
                    HCNode* node_R = myStack.top();  // right child
                    myStack.pop();
                    HCNode* node_L = myStack.top();  // left child
                    myStack.pop();

                    HCNode* newParent =
                        new HCNode(0, node_L->symbol, node_L, node_R, 0);
                    node_R->p = newParent;
                    node_L->p = newParent;

                    myStack.push(newParent);
                }
                // one item left -> the root
                else if (myStack.size() == 1) {
                    root = myStack.top();
                    myStack.pop();
                    break;
                }
            }

        } else {
            char symbol = in.readSymbol();
            // std::cout << symbol << std::endl;
            HCNode* newNode = new HCNode(0, symbol, 0, 0, 0);
            myStack.push(newNode);
            next_bit = true;
        }
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
    std::stack<unsigned int> rev_bit;
    // the ascii code of symbol is the position of the vector leaves
    int pos = (int)symbol;
    HCNode* curr = leaves[pos];

    // if there is only one node in the HCTree
    if (curr == root) rev_bit.push(0);
    // iterate through the leave to the root
    while (curr != root) {
        if (curr == curr->p->c0)
            rev_bit.push(0);
        else
            rev_bit.push(1);
        curr = curr->p;
    }
    // output the result
    string ans;
    while (!rev_bit.empty()) {
        unsigned int ans = rev_bit.top();
        out.writeBit(ans);
        rev_bit.pop();
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
    HCNode* node = leaves[symbol];
    string encodedStr = "";

    // if leaf's parent is its parent's c0 child
    // return 0
    // otherwise return 1
    while (node != root) {
        if (node == node->p->c0) {
            encodedStr = '0' + encodedStr;
        } else {
            encodedStr = '1' + encodedStr;
        }
        node = node->p;
    }

    // if node is root
    if (encodedStr == "") {
        out << '0';
    } else {
        out << encodedStr;
    }
}

/**
 * TODO: Decode the sequence of bits (represented as a char of either '0' or
 * '1') from the istream to return the coded symbol. For this function to work,
 * build() must have been called beforehand to create the HCTree.
 */
byte HCTree::decode(BitInputStream& in) const {
    // HCNode* curr = root;

    // // if there is only one node : every decode is root's symbol
    // if (root->c0 == 0 and root->c1 == 0) {
    //     unsigned int tmp = in.readBit();
    //     return curr->symbol;
    // }
    // while (curr->c0 != 0 and curr->c1 != 0) {
    //     unsigned int tmp = in.readBit();
    //     if (tmp == 0)
    //         curr = curr->c0;
    //     else
    //         curr = curr->c1;
    // }
    // return curr->symbol;

    HCNode* node = root;

    while (node->c0 != nullptr || node->c1 != nullptr) {
        unsigned int nextBit = in.readBit();
        if (nextBit == 0 && node->c0 != nullptr) {
            node = node->c0;
        } else if (nextBit == 1 && node->c1 != nullptr) {
            node = node->c1;
        }
    }
    return node->symbol;
}

/**
 * TODO: Decode the sequence of bits (represented as char of either '0' or '1')
 * from istream to return the coded symbol. For this function to work, build()
 * must have been called beforehand to create the HCTree.
 */
byte HCTree::decode(istream& in) const {
    HCNode* node = root;
    char c;

    // if root only has one node
    if (node->c0 == nullptr && node->c1 == nullptr) {
        in.get(c);
        return node->symbol;
    }

    // loop getting single characters
    while (in.get(c)) {
        if (c == '0') {
            node = node->c0;
        } else {
            node = node->c1;
        }

        // if get leaf node
        if (node->c0 == nullptr && node->c1 == nullptr) {
            return node->symbol;
        }
    }
}
