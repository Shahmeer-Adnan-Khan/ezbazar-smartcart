#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include <iostream>

class Product {
private:
    std::string category;
    int productID;
    std::string name;
    double price;
    int stockQuantity;
    int cartQuantity; // Tracks how many units of this item are in a customer's cart

public:
    // Constructors
    Product();
    Product(std::string cat, int id, std::string n, double p, int stock, int cartQty = 1);

    // Getters (const-qualified so they cannot accidentally modify member data)
    std::string getCategory() const;
    int getProductID() const;
    std::string getName() const;
    double getPrice() const;
    int getStockQuantity() const;
    int getCartQuantity() const;

    // Setters
    void setStockQuantity(int newStock);
    void setCartQuantity(int newCartQty);

    // Display Helpers
    void displayProduct() const;     // Used when viewing store Inventory
    void displayCartItem() const;    // Used when viewing SmartCart (shows Qty & Subtotal)
};

#endif 
