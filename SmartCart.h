#ifndef SMARTCART_H
#define SMARTCART_H

#include <string>
#include <vector>
#include "Product.h"
#include "CartList.h"

class SmartCart {
private:
    CartList cartItems; // Custom Doubly Linked List replaces std::vector<Product>

public:
    SmartCart();

    void addItem(const Product& p, int quantity = 1);
    bool removeItem(int productID, Product& removedOut);
    void viewCart() const;
    bool isEmpty() const;
    std::vector<Product> getItemsVector() const;
    bool checkout(const std::string& customerName);
};

#endif 
