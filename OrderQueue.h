#ifndef ORDERQUEUE_H
#define ORDERQUEUE_H

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "Product.h"

// Represents a single customer checkout order
struct Order {
    int orderID;
    std::string customerName;
    std::vector<Product> items;
    double totalAmount;

    Order() : orderID(0), customerName(""), totalAmount(0.0) {}
    Order(int id, std::string name, const std::vector<Product>& i, double total)
        : orderID(id), customerName(name), items(i), totalAmount(total) {}
};

// Node for our FIFO Order Queue
struct QueueNode {
    Order data;
    QueueNode* next;

    QueueNode(const Order& o) : data(o), next(nullptr) {}
};

class OrderQueue {
private:
    QueueNode* frontNode;
    QueueNode* rearNode;
    int count;
    int nextOrderID;

    // Prevent accidental shallow copying
    OrderQueue(const OrderQueue&) = delete;
    void operator=(const OrderQueue&) = delete;

public:
    OrderQueue() : frontNode(nullptr), rearNode(nullptr), count(0), nextOrderID(1001) {}

    ~OrderQueue() {
        Order dummy;
        while (!isEmpty()) {
            dequeue(dummy);
        }
    }

    // Singleton instance so both Customer Checkout and Manager Dashboard share the queue
    static OrderQueue& getInstance() {
        static OrderQueue instance;
        return instance;
    }

    // Enqueues a new order at the rear in O(1) time
    int enqueueOrder(const std::string& customerName, const std::vector<Product>& items, double total) {
        Order newOrder(nextOrderID++, customerName, items, total);
        QueueNode* newNode = new QueueNode(newOrder);

        if (rearNode) {
            rearNode->next = newNode;
        }
        rearNode = newNode;

        if (!frontNode) {
            frontNode = rearNode; // First order in the queue
        }
        count++;
        return newOrder.orderID;
    }

    // Dequeues the oldest order from the front in O(1) time
    bool dequeue(Order& outOrder) {
        if (isEmpty()) return false;

        QueueNode* temp = frontNode;
        outOrder = temp->data;
        frontNode = frontNode->next;

        if (!frontNode) {
            rearNode = nullptr; // Queue is now completely empty
        }

        delete temp;
        count--;
        return true;
    }

    // Peeks at the front order without removing it (O(1))
    const Order* peek() const {
        if (isEmpty()) return nullptr;
        return &(frontNode->data);
    }

    // Displays all pending orders waiting in the FIFO queue
    void displayPendingOrders() const {
        std::cout << "\n==================================================================\n";
        std::cout << "             PENDING CUSTOMER ORDERS (FIFO Queue: " << count << ")              \n";
        std::cout << "==================================================================\n";
        if (isEmpty()) {
            std::cout << "  No pending orders in the queue.\n";
            std::cout << "==================================================================\n";
            return;
        }

        QueueNode* curr = frontNode;
        int pos = 1;
        while (curr) {
            std::cout << "  #" << pos++ << " | Order ID: " << curr->data.orderID
                      << " | Customer: " << std::left << std::setw(15) << curr->data.customerName
                      << " | Items: " << curr->data.items.size()
                      << " | Total: Rs. " << std::fixed << std::setprecision(2) << curr->data.totalAmount << "\n";
            curr = curr->next;
        }
        std::cout << "==================================================================\n";
    }

    bool isEmpty() const { return frontNode == nullptr; }
    int getSize() const { return count; }
};

#endif 
