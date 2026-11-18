#ifndef CARTLIST_H
#define CARTLIST_H

#include <iostream>
#include <vector>
#include <iomanip>
#include "Product.h"

// Node for our Doubly Linked List
struct CartNode {
    Product data;
    CartNode* next;
    CartNode* prev;

    CartNode(const Product& p) : data(p), next(nullptr), prev(nullptr) {}
};

class CartList {
private:
    CartNode* head;
    CartNode* tail;
    int uniqueItemCount;

public:
    CartList() : head(nullptr), tail(nullptr), uniqueItemCount(0) {}

    // Copy Constructor (Rule of Three: ensures safe deep copying if CartList is copied)
    CartList(const CartList& other) : head(nullptr), tail(nullptr), uniqueItemCount(0) {
        CartNode* curr = other.head;
        while (curr) {
            appendOrIncrement(curr->data, curr->data.getCartQuantity());
            curr = curr->next;
        }
    }

    // Copy Assignment Operator
    CartList& operator=(const CartList& other) {
        if (this != &other) {
            clear();
            CartNode* curr = other.head;
            while (curr) {
                appendOrIncrement(curr->data, curr->data.getCartQuantity());
                curr = curr->next;
            }
        }
        return *this;
    }

    ~CartList() {
        clear();
    }

    // Adds a product to the cart. If the Product ID already exists in the list,
    // it increments its cartQuantity instead of creating a duplicate node!
    void appendOrIncrement(const Product& p, int qtyToAdd = 1) {
        if (qtyToAdd <= 0) return;

        // Step 1: Check if product is already in the cart
        CartNode* current = head;
        while (current) {
            if (current->data.getProductID() == p.getProductID()) {
                current->data.setCartQuantity(current->data.getCartQuantity() + qtyToAdd);
                return;
            }
            current = current->next;
        }

        // Step 2: Not found, so allocate a new Doubly Linked Node at the tail (O(1))
        Product itemCopy = p;
        itemCopy.setCartQuantity(qtyToAdd);
        CartNode* newNode = new CartNode(itemCopy);

        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        uniqueItemCount++;
    }

    // Removes an item node by ID and outputs the removed Product (with its cartQuantity)
    // so the caller can return that stock back to the Inventory!
    bool removeByID(int id, Product& removedProductOut) {
        CartNode* current = head;
        while (current) {
            if (current->data.getProductID() == id) {
                removedProductOut = current->data;

                // Relink previous neighbor
                if (current->prev) {
                    current->prev->next = current->next;
                } else {
                    head = current->next; // Removing head node
                }

                // Relink next neighbor
                if (current->next) {
                    current->next->prev = current->prev;
                } else {
                    tail = current->prev; // Removing tail node
                }

                delete current;
                uniqueItemCount--;
                return true;
            }
            current = current->next;
        }
        return false; // ID not found in cart
    }

    // Calculates total bill dynamically by traversing the Doubly Linked List
    double getTotalAmount() const {
        double total = 0.0;
        CartNode* current = head;
        while (current) {
            total += current->data.getPrice() * current->data.getCartQuantity();
            current = current->next;
        }
        return total;
    }

    // Displays all items from head to tail
    void displayCart() const {
        std::cout << "\n==================================================================\n";
        std::cout << "                   YOUR SMARTCART (Doubly Linked List)            \n";
        std::cout << "==================================================================\n";
        if (!head) {
            std::cout << "  Your cart is currently empty.\n";
            std::cout << "==================================================================\n";
            return;
        }

        CartNode* current = head;
        while (current) {
            current->data.displayCartItem();
            current = current->next;
        }
        std::cout << "------------------------------------------------------------------\n";
        std::cout << "  Unique Items: " << uniqueItemCount
                  << " | Grand Total: Rs. " << std::fixed << std::setprecision(2)
                  << getTotalAmount() << "\n";
        std::cout << "==================================================================\n";
    }

    // Frees all nodes in O(N) time
    void clear() {
        CartNode* current = head;
        while (current) {
            CartNode* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = tail = nullptr;
        uniqueItemCount = 0;
    }

    bool isEmpty() const { return head == nullptr; }
    int getSize() const { return uniqueItemCount; }

    // Exports items to a vector for receipt generation or order queueing
    std::vector<Product> toVector() const {
        std::vector<Product> vec;
        CartNode* current = head;
        while (current) {
            vec.push_back(current->data);
            current = current->next;
        }
        return vec;
    }
};

#endif
