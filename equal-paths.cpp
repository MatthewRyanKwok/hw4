#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool checkEqualPaths(Node* root, int depth, int& lDepth) {
    if (!root) { //if current node null, returns true
        return true;
    }

    if (!root->left && !root->right) { //checks if it is a leaf 
        if (lDepth == -1) { //stores depth if it is the first leaf found
            lDepth = depth;
        }
        return depth == lDepth;
    }
    bool rCheck = true; //checks right tree
    bool lCheck = true; //checks left tree

    if (root->left != nullptr) { //check left subtree, increases depth
        lCheck = checkEqualPaths(root->left, depth + 1, lDepth);
    }

    if (root->right != nullptr) { //check right subtree, increase depth
        rCheck = checkEqualPaths(root->right, depth + 1, lDepth);
    }
    return lCheck && rCheck; //both subtrees have to have equal paths
}

bool equalPaths(Node * root)
{
    // Add your code below
    int lDepth = -1;
    return checkEqualPaths(root, 0, lDepth);

}

