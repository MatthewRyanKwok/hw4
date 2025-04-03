#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rebalanceAfterInsert(AVLNode<Key, Value>* node);
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void rebalanceAfterRemove(AVLNode<Key, Value>* node);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr); //new node allocated

    if (!this->root_) { //empty tree means new node is root
        this->root_ = newNode;
        return;
    }

    AVLNode<Key, Value>* parent = static_cast<AVLNode<Key, Value>*>(this->root_); 
    AVLNode<Key, Value>* current = parent;

    while (current) { //traverse tree
        parent = current;
        if (newNode->getKey() < current->getKey()) {
            current = current->getLeft();
        }
        else if (newNode->getKey() > current->getKey()) {
            current = current->getRight();
        }
        else { //if the key exists the node value is updates 
            current->setValue(newNode->getValue());
            delete newNode;
            return;
        }
    }

    newNode->setParent(parent); //new node attached to parent
    if (newNode->getKey() < parent->getKey()) {
        parent->setLeft(newNode);
    }
    else {
        parent->setRight(newNode);
    }

    rebalanceAfterInsert(newNode); //rebalance tree
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (node == nullptr) { //if no key found, nothing is to be removed
        return;  
    }

    AVLNode<Key, Value>* parent = node->getParent(); //the parent of the node is gotten

    //if node has two children, swap w pred
    if (node->getLeft() != nullptr && node->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        nodeSwap(node, pred); //swap w pred
        parent = node->getParent(); //parent pointer updated
    }

    //node should have one child at most
    AVLNode<Key, Value>* child = nullptr;
    if (node->getLeft() != nullptr) {
        child = node->getLeft();
    }
    else if (node->getRight() != nullptr) {
        child = node->getRight();
    }

    //if the child exists, parent pointer updated
    if (child != nullptr) {
        child->setParent(parent);
    }

    if (parent == nullptr) { //update ptr
        this->root_ = child;
    }
    else {
        if (parent->getLeft() == node) {
            parent->setLeft(child);
        }
        else {
            parent->setRight(child);
        }
    }

    delete node;

    rebalanceAfterRemove(parent); //rebalances tree starts from parent
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceAfterRemove(AVLNode<Key, Value>* node)
{
    while (node != nullptr) {
        int prevBalance = node->getBalance(); //stores the previous balanced

        if (node->getLeft() != nullptr && node->getRight() == nullptr) {
            node->updateBalance(1); //updates the balance to 1
        }
        else if (node->getRight() != nullptr && node->getLeft() == nullptr) {
            node->updateBalance(-1); //updates the balance to -1
        }

        if (node->getBalance() == 2) { //does a check if balancing is needed
            AVLNode<Key, Value>* rightChild = node->getRight();
            if (rightChild->getBalance() < 0) {
                rotateRight(rightChild);
            }
            rotateLeft(node);
        }
        else if (node->getBalance() == -2) {
            AVLNode<Key, Value>* leftChild = node->getLeft();
            if (leftChild->getBalance() > 0) {
                rotateLeft(leftChild);
            }
            rotateRight(node);
        }

        if (prevBalance == node->getBalance()) { //if the balance remains the same, there is no more balancing required
            break;
        }

        node = node->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceAfterInsert(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* parent = node->getParent();
    while (parent) {
        if (node == parent->getLeft()) { //updates parent's balance depending on the insertion (l/r subtree)
            parent->updateBalance(-1);
        }
        else {
            parent->updateBalance(1);
        }

        if (parent->getBalance() == 0) { //no balancing needed if balance factor is 0
            break;
        }
        else if (parent->getBalance() == -2) { //if it is -2, rotate
            if (node->getBalance() == 1) {
                rotateLeft(node);
            }
            rotateRight(parent);
            break;
        }
        else if (parent->getBalance() == 2) { //if it is 2 rotate
            if (node->getBalance() == -1) {
                rotateRight(node);
            }
            rotateLeft(parent);
            break;
        }
        node = parent;
        parent = parent->getParent();
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* rightChild = node->getRight();
    if (!rightChild) return;

    node->setRight(rightChild->getLeft()); //node right child is right child's left child
    if (rightChild->getLeft()) {
        rightChild->getLeft()->setParent(node);
    }

    rightChild->setParent(node->getParent()); //right child's parent is updated
    if (!node->getParent()) {
        this->root_ = rightChild;
    }
    else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(rightChild);
    }
    else {
        node->getParent()->setRight(rightChild);
    }

    rightChild->setLeft(node); //node is left child of rightCHild
    node->setParent(rightChild);

    //balance factors updated
    node->setBalance(node->getBalance() - 1 - std::max(0, static_cast<int>(rightChild->getBalance())));
    rightChild->setBalance(rightChild->getBalance() - 1 + std::min(static_cast<int8_t>(0), node->getBalance()));
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* leftChild = node->getLeft();
    if (!leftChild) return;

    node->setLeft(leftChild->getRight()); //node's left child is left child's right child
    if (leftChild->getRight()) {
        leftChild->getRight()->setParent(node);
    }

    leftChild->setParent(node->getParent()); //left child's parent is updated
    if (!node->getParent()) {
        this->root_ = leftChild;
    }
    else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(leftChild);
    }
    else {
        node->getParent()->setRight(leftChild);
    }

    leftChild->setRight(node); //node is now right child of left child
    node->setParent(leftChild);

    //updates balance
    node->setBalance(node->getBalance() + 1 - std::min(0, static_cast<int>(leftChild->getBalance())));
    leftChild->setBalance(leftChild->getBalance() + 1 + std::max(static_cast<int8_t>(0), node->getBalance()));
}



#endif