#ifndef INVENTORY_H
#define INVENTORY_H

#include <iostream>
#include <string>
#include <vector>
#include "Product.h"
#include "AVLTree.h"

class Inventory {
private:
    // Custom Self-Balancing AVL Tree replaces the old std::map<string, vector<Product>>
    ProductAVLTree stockTree;

    // Private Constructor for Singleton Design Pattern
    Inventory() {}

    // Seeds default store items if inventory.txt is missing
    void seedDefaultData();

public:
    Inventory(const Inventory&) = delete;
    void operator=(const Inventory&) = delete;

    static Inventory& getInstance() {
        static Inventory instance;
        return instance;
    }

    // Core AVL Tree Inventory Operations
    void addProduct(const Product& p);
    bool getProduct(int id, Product& outProduct) const;
    bool removeProduct(int id);
    bool reduceStock(int id, int amount);
    void increaseStock(int id, int amount);
    bool productExists(int id) const;
    bool changeStock(int id, int newStock);

    // Display & Filtering Operations
    void displayInventory() const;
    void displayByCategory(const std::string& category) const;
    void searchByNameSubstring(const std::string& keyword) const;

    // File Persistence
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
};

#endif 
