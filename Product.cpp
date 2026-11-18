#include "Product.h"
#include <iomanip>

// Default constructor initializes safe blank values
Product::Product() {
    category = "Unknown";
    productID = 0;
    name = "Unnamed";
    price = 0.0;
    stockQuantity = 0;
    cartQuantity = 0;
}

// Parameterized constructor used when loading from file or adding products
Product::Product(std::string cat, int id, std::string n, double p, int stock, int cartQty) {
    category = cat;
    productID = id;
    name = n;
    price = p;
    stockQuantity = (stock >= 0) ? stock : 0;
    cartQuantity = (cartQty >= 0) ? cartQty : 1;
}

// --- Getters ---
std::string Product::getCategory() const { return category; }
int Product::getProductID() const { return productID; }
std::string Product::getName() const { return name; }
double Product::getPrice() const { return price; }
int Product::getStockQuantity() const { return stockQuantity; }
int Product::getCartQuantity() const { return cartQuantity; }

// --- Setters ---
void Product::setStockQuantity(int newStock) {
    stockQuantity = (newStock >= 0) ? newStock : 0;
}

void Product::setCartQuantity(int newCartQty) {
    cartQuantity = (newCartQty >= 0) ? newCartQty : 0;
}

// --- Display Methods ---
void Product::displayProduct() const {
    std::cout << "  [" << std::setw(3) << productID << "] "
              << std::left << std::setw(24) << name
              << " | Cat: " << std::setw(12) << category
              << " | Price: Rs. " << std::right << std::setw(8) << std::fixed << std::setprecision(2) << price
              << " | Stock: " << std::setw(4) << stockQuantity << "\n";
}

void Product::displayCartItem() const {
    double subtotal = price * cartQuantity;
    std::cout << "  [" << std::setw(3) << productID << "] "
              << std::left << std::setw(22) << name
              << " | Rs. " << std::right << std::setw(8) << std::fixed << std::setprecision(2) << price
              << " x " << std::setw(2) << cartQuantity
              << " = Rs. " << std::setw(9) << subtotal << "\n";
}
