#ifndef AVLTREE_H
#define AVLTREE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "Product.h"

// Node structure for the AVL Tree
struct AVLNode {
    Product data;       // The store product stored in this node
    AVLNode* left;      // Pointer to left child (smaller Product IDs)
    AVLNode* right;     // Pointer to right child (larger Product IDs)
    int height;         // Height of this node in the tree (leaf node = 1)

    AVLNode(const Product& p)
        : data(p), left(nullptr), right(nullptr), height(1) {}
};

class ProductAVLTree {
private:
    AVLNode* root;

    // Helper 1: Safely returns the height of a node (0 if nullptr)
    int getHeight(AVLNode* node) const {
        return node ? node->height : 0;
    }

    // Helper 2: Calculates Balance Factor = Height(Left) - Height(Right)
    int getBalanceFactor(AVLNode* node) const {
        return node ? (getHeight(node->left) - getHeight(node->right)) : 0;
    }

    // Helper 3: Updates a node's height based on its tallest child
    void updateHeight(AVLNode* node) {
        if (node) {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
        }
    }

    // Right Rotate (Used to fix Left-Left heavy imbalance)

    AVLNode* rightRotate(AVLNode* y) {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;

        // Perform rotation
        x->right = y;
        y->left = T2;

        // Update heights (must update child 'y' first, then new root 'x')
        updateHeight(y);
        updateHeight(x);

        return x; // New root of this subtree
    }

    // Left Rotate (Used to fix Right-Right heavy imbalance)
 
    AVLNode* leftRotate(AVLNode* x) {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;

        // Perform rotation
        y->left = x;
        x->right = T2;

        // Update heights (must update child 'x' first, then new root 'y')
        updateHeight(x);
        updateHeight(y);

        return y; // New root of this subtree
    }

    // Rebalances a node if its Balance Factor is outside [-1, 0, +1]
    AVLNode* rebalance(AVLNode* node) {
        updateHeight(node);
        int balance = getBalanceFactor(node);

        // Case 1: Left-Left (LL) Heavy -> Single Right Rotation
        if (balance > 1 && getBalanceFactor(node->left) >= 0) {
            return rightRotate(node);
        }

        // Case 2: Left-Right (LR) Heavy -> Left Rotate left child, then Right Rotate node
        if (balance > 1 && getBalanceFactor(node->left) < 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Case 3: Right-Right (RR) Heavy -> Single Left Rotation
        if (balance < -1 && getBalanceFactor(node->right) <= 0) {
            return leftRotate(node);
        }

        // Case 4: Right-Left (RL) Heavy -> Right Rotate right child, then Left Rotate node
        if (balance < -1 && getBalanceFactor(node->right) > 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node; // Already balanced
    }

    // Recursive insertion with automatic AVL rebalancing
    AVLNode* insertNode(AVLNode* node, const Product& p) {
        // 1. Standard BST Insertion
        if (!node) return new AVLNode(p);

        if (p.getProductID() < node->data.getProductID()) {
            node->left = insertNode(node->left, p);
        } else if (p.getProductID() > node->data.getProductID()) {
            node->right = insertNode(node->right, p);
        } else {
            // Duplicate ID: update existing product data
            node->data = p;
            return node;
        }

        // 2. Update height and rebalance on the way back up the recursion stack
        return rebalance(node);
    }

    // Finds the node with the minimum Product ID in a subtree (leftmost node)
    AVLNode* findMin(AVLNode* node) const {
        while (node && node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    // Recursive removal with automatic AVL rebalancing
    AVLNode* removeNode(AVLNode* node, int id, bool& success) {
        if (!node) return nullptr;

        if (id < node->data.getProductID()) {
            node->left = removeNode(node->left, id, success);
        } else if (id > node->data.getProductID()) {
            node->right = removeNode(node->right, id, success);
        } else {
            // Node found!
            success = true;

            // Case A: Node has 0 or 1 child
            if (!node->left || !node->right) {
                AVLNode* temp = node->left ? node->left : node->right;
                delete node;
                return temp;
            }

            // Case B: Node has 2 children
            // Get the in-order successor (smallest ID in the right subtree)
            AVLNode* successor = findMin(node->right);
            node->data = successor->data;
            // Delete the in-order successor from the right subtree
            bool dummy = false;
            node->right = removeNode(node->right, successor->data.getProductID(), dummy);
        }

        // Rebalance the tree on the way back up
        return rebalance(node);
    }

    // O(log N) recursive search returning a pointer to the actual node
    AVLNode* searchNode(AVLNode* node, int id) const {
        if (!node) return nullptr;
        if (id == node->data.getProductID()) return node;
        if (id < node->data.getProductID()) return searchNode(node->left, id);
        return searchNode(node->right, id);
    }

    // In-Order Traversal (Left -> Root -> Right) visits products in ascending ID order!
    void inOrderTraversal(AVLNode* node, std::vector<Product>& outList) const {
        if (node) {
            inOrderTraversal(node->left, outList);
            outList.push_back(node->data);
            inOrderTraversal(node->right, outList);
        }
    }

    // Post-Order Traversal (Left -> Right -> Root) safely frees all memory
    void destroyTree(AVLNode* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    ProductAVLTree() : root(nullptr) {}
    ~ProductAVLTree() { destroyTree(root); }

    // Inserts a product in O(log N) time
    void insert(const Product& p) {
        root = insertNode(root, p);
    }

    // Removes a product by ID in O(log N) time
    bool remove(int id) {
        bool success = false;
        root = removeNode(root, id, success);
        return success;
    }

    // Searches for a product by ID in O(log N) time and copies it to outProduct
    bool search(int id, Product& outProduct) const {
        AVLNode* found = searchNode(root, id);
        if (found) {
            outProduct = found->data;
            return true;
        }
        return false;
    }

    // Direct pointer lookup so Inventory can update stock in-place in O(log N) time
    Product* findProductPtr(int id) {
        AVLNode* found = searchNode(root, id);
        return found ? &(found->data) : nullptr;
    }

    bool isEmpty() const {
        return root == nullptr;
    }

    int getTreeHeight() const {
        return getHeight(root);
    }

    // Uses STL vector strictly as a return buffer for displaying or saving
    std::vector<Product> getAllProductsSorted() const {
        std::vector<Product> list;
        inOrderTraversal(root, list);
        return list;
    }
};

#endif 
