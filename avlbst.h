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
    void rotateRight(AVLNode<Key, Value>* y);
    void rotateLeft(AVLNode<Key, Value>* y);

    // height of a node 
    int height(AVLNode<Key, Value>* node) {
        return node ? node->getBalance() : -1;
    }

    // balance factor for a node
    int balanceFactor(AVLNode<Key, Value>* node) {
        if (!node) return 0;
        return height(node->getLeft()) - height(node->getRight());
    }

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    Key key = new_item.first;
    Value value = new_item.second;

    // empty.
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(key, value, nullptr);
        return;
    }

    // traverse the tree to find insertion point
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;
    while (current != nullptr) {
        parent = current;
        if (key < current->getKey())
            current = static_cast<AVLNode<Key, Value>*>(current->getLeft());
        else if (key > current->getKey())
            current = static_cast<AVLNode<Key, Value>*>(current->getRight());
        else {
            // key already exists so update the value.
            current->setValue(value);
            return;
        }
    }

    // insert new node as a child of parent
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(key, value, parent);
    if (key < parent->getKey())
        parent->setLeft(newNode);
    else
        parent->setRight(newNode);

    //std::cout << "Inserted node: " << newNode->getKey() << std::endl;

    // update heights and check for imbalances walking up from  new node
    AVLNode<Key, Value>* node = newNode;
    while (node != nullptr) {
        int leftH = height(static_cast<AVLNode<Key, Value>*>(node->getLeft()));
        int rightH = height(static_cast<AVLNode<Key, Value>*>(node->getRight()));
        int newHeight = 1 + std::max(leftH, rightH);
        node->setBalance(newHeight);
        int bf = leftH - rightH; // balance factor

        //std::cout << "Node: " << node->getKey() 
                  // << " | Left Height: " << leftH 
                  // << " | Right Height: " << rightH
                  // << " | Height: " << newHeight 
                  // << " | Balance Factor: " << bf << std::endl;

        // check for imbalances
        if (bf > 1) { // left heavy
            //std::cout << "Left Heavy at node: " << node->getKey() << std::endl;
            // leftright case
            if (balanceFactor(static_cast<AVLNode<Key, Value>*>(node->getLeft())) < 0) {
                //std::cout << "Performing Left-Right Rotation at node: " << node->getKey() << std::endl;
                rotateLeft(static_cast<AVLNode<Key, Value>*>(node->getLeft()));
            }
            //std::cout << "Performing Right Rotation at node: " << node->getKey() << std::endl;
            rotateRight(node);
            break;
        }
        else if (bf < -1) { // right heavy
            //std::cout << "Right Heavy at node: " << node->getKey() << std::endl;
            // right left case
            if (balanceFactor(static_cast<AVLNode<Key, Value>*>(node->getRight())) > 0) {
                //std::cout << "Performing Right-Left Rotation at node: " << node->getKey() << std::endl;
                rotateRight(static_cast<AVLNode<Key, Value>*>(node->getRight()));
            }
            //std::cout << "Performing Left Rotation at node: " << node->getKey() << std::endl;
            rotateLeft(node);
            break;
        }

        node = static_cast<AVLNode<Key, Value>*>(node->getParent());
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    // locate the node to be removed
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (node == nullptr)
        return;  // key not found

    // if node has two children, swap with predecessor
    if (node->getLeft() != nullptr && node->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(node));
        nodeSwap(node, pred);
    }

    // node has at most one child
    AVLNode<Key, Value>* child = static_cast<AVLNode<Key, Value>*>(
        (node->getLeft() != nullptr) ? node->getLeft() : node->getRight()
    );
    AVLNode<Key, Value>* parent = node->getParent();

    // link the child to parent
    if (child != nullptr)
        child->setParent(parent);

    if (parent == nullptr) { // node is the root
        this->root_ = child;
    }
    else {
        if (node == parent->getLeft())
            parent->setLeft(child);
        else
            parent->setRight(child);
    }

    // save parent pointer for rebalancing
    AVLNode<Key, Value>* rebalanceNode = parent;
    delete node; // free removed node.

    // rebalance  tree upward from the removed node parent
    while (rebalanceNode != nullptr) {
        int leftH = height(static_cast<AVLNode<Key, Value>*>(rebalanceNode->getLeft()));
        int rightH = height(static_cast<AVLNode<Key, Value>*>(rebalanceNode->getRight()));
        int newHeight = 1 + std::max(leftH, rightH);
        rebalanceNode->setBalance(newHeight);
        int bf = leftH - rightH;  // compute bf

        //std::cout << "Rebalancing at node: " << rebalanceNode->getKey() 
                  // << " | Left Height: " << leftH 
                  // << " | Right Height: " << rightH 
                  // << " | New Height: " << newHeight 
                  // << " | Balance Factor: " << bf << std::endl;

        // left heavy
        if (bf > 1) {
            // leftright case
            if (balanceFactor(static_cast<AVLNode<Key, Value>*>(rebalanceNode->getLeft())) < 0) {
                // std::cout << "Performing Left-Right Rotation at node: " << rebalanceNode->getKey() << std::endl;
                rotateLeft(static_cast<AVLNode<Key, Value>*>(rebalanceNode->getLeft()));
            }
            // std::cout << "Performing Right Rotation at node: " << rebalanceNode->getKey() << std::endl;
            rotateRight(rebalanceNode);
        }
        // Right heavy.
        else if (bf < -1) {
            // check for right left case
            if (balanceFactor(static_cast<AVLNode<Key, Value>*>(rebalanceNode->getRight())) > 0) {
                // std::cout << "Performing Right-Left Rotation at node: " << rebalanceNode->getKey() << std::endl;
                rotateRight(static_cast<AVLNode<Key, Value>*>(rebalanceNode->getRight()));
            }
            // std::cout << "Performing Left Rotation at node: " << rebalanceNode->getKey() << std::endl;
            rotateLeft(rebalanceNode);
        }

        // move upward in tree
        rebalanceNode = static_cast<AVLNode<Key, Value>*>(rebalanceNode->getParent());
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template <class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* rightChild = static_cast<AVLNode<Key, Value>*>(node->getRight());
    //std::cout << "Rotating left at node: " << node->getKey() << std::endl;
    if (rightChild == nullptr) {
        //std::cout << "Error: Cannot rotate left because right child is nullptr!" << std::endl;
        return;
    }

    // perform rotation
    node->setRight(rightChild->getLeft());
    if (rightChild->getLeft() != nullptr){
        rightChild->getLeft()->setParent(node);
    }
    rightChild->setParent(node->getParent());
    if (node->getParent() == nullptr) {
        this->root_ = rightChild;
        //std::cout << "Updated root to: " << rightChild->getKey() << std::endl;
    }
    else if (node == node->getParent()->getLeft()){
        node->getParent()->setLeft(rightChild);
    }else{
        node->getParent()->setRight(rightChild);
    }
    rightChild->setLeft(node);
    node->setParent(rightChild);

    // update height
    int nodeLeftH = height(static_cast<AVLNode<Key, Value>*>(node->getLeft()));
    int nodeRightH = height(static_cast<AVLNode<Key, Value>*>(node->getRight()));
    node->setBalance(1 + std::max(nodeLeftH, nodeRightH));

    int rightLeftH = height(static_cast<AVLNode<Key, Value>*>(rightChild->getLeft()));
    int rightRightH = height(static_cast<AVLNode<Key, Value>*>(rightChild->getRight()));
    rightChild->setBalance(1 + std::max(rightLeftH, rightRightH));

    //std::cout << "Left rotation completed. New subtree root: " << rightChild->getKey() << std::endl;
}


template <class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    if (node == nullptr || node->getLeft() == nullptr) {
        //std::cout << "Node or its left child is nullptr, cannot rotate right!" << std::endl;
        return;
    }

    AVLNode<Key, Value>* leftChild = static_cast<AVLNode<Key, Value>*>(node->getLeft());
    //std::cout << "Rotating right at node: " << node->getKey() << std::endl;

    // Perform rotation.
    node->setLeft(leftChild->getRight());
    if (leftChild->getRight() != nullptr){
      leftChild->getRight()->setParent(node);
    }
        

    leftChild->setParent(node->getParent());
    if (node->getParent() == nullptr){
        this->root_ = leftChild;
    }else if (node == node->getParent()->getRight()){
        node->getParent()->setRight(leftChild);
    }else{
        node->getParent()->setLeft(leftChild);
    }

    leftChild->setRight(node);
    node->setParent(leftChild);

    // update heights
    int nodeLeftH = height(static_cast<AVLNode<Key, Value>*>(node->getLeft()));
    int nodeRightH = height(static_cast<AVLNode<Key, Value>*>(node->getRight()));
    node->setBalance(1 + std::max(nodeLeftH, nodeRightH));

    int leftLeftH = height(static_cast<AVLNode<Key, Value>*>(leftChild->getLeft()));
    int leftRightH = height(static_cast<AVLNode<Key, Value>*>(leftChild->getRight()));
    leftChild->setBalance(1 + std::max(leftLeftH, leftRightH));

    //std::cout << "Right rotation completed. New subtree root: " << leftChild->getKey() << std::endl;
}


#endif
