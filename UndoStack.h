#ifndef UNDOSTACK_H
#define UNDOSTACK_H

#include <iostream>
#include <string>
#include "Product.h"

enum ActionType {
    ACTION_ADD_PRODUCT,
    ACTION_REMOVE_PRODUCT,
    ACTION_CHANGE_STOCK
};

// Stores the snapshot needed to reverse a manager action
struct Action {
    ActionType type;
    Product productSnapshot; // Copy of the product before/during the action
    int oldStock;            // Previous stock level (used for ACTION_CHANGE_STOCK)
    int newStock;            // New stock level

    Action(ActionType t, const Product& p, int oldS = 0, int newS = 0)
        : type(t), productSnapshot(p), oldStock(oldS), newStock(newS) {}
};

// Node for our LIFO Stack
struct StackNode {
    Action data;
    StackNode* next;

    StackNode(const Action& a) : data(a), next(nullptr) {}
};

class UndoStack {
private:
    StackNode* topNode;
    int count;

    UndoStack(const UndoStack&) = delete;
    void operator=(const UndoStack&) = delete;

public:
    UndoStack() : topNode(nullptr), count(0) {}

    ~UndoStack() {
        Action dummy(ACTION_ADD_PRODUCT, Product());
        while (!isEmpty()) {
            pop(dummy);
        }
    }

    // Push an action onto the top of the stack in O(1) time
    void push(const Action& a) {
        StackNode* newNode = new StackNode(a);
        newNode->next = topNode;
        topNode = newNode;
        count++;
    }

    // Pop the most recent action from the top of the stack in O(1) time
    bool pop(Action& outAction) {
        if (isEmpty()) return false;

        StackNode* temp = topNode;
        outAction = temp->data;
        topNode = topNode->next;
        delete temp;
        count--;
        return true;
    }

    // Peek at the top action without removing it
    const Action* peek() const {
        if (isEmpty()) return nullptr;
        return &(topNode->data);
    }

    bool isEmpty() const { return topNode == nullptr; }
    int getSize() const { return count; }
};

#endif 
