#include "SmartCart.h"
#include "OrderQueue.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>

SmartCart::SmartCart() {}

void SmartCart::addItem(const Product& p, int quantity) {
    cartItems.appendOrIncrement(p, quantity);
}

bool SmartCart::removeItem(int productID, Product& removedOut) {
    return cartItems.removeByID(productID, removedOut);
}

void SmartCart::viewCart() const {
    cartItems.displayCart();
}

bool SmartCart::isEmpty() const {
    return cartItems.isEmpty();
}

std::vector<Product> SmartCart::getItemsVector() const {
    return cartItems.toVector();
}

bool SmartCart::checkout(const std::string& customerName) {
    if (cartItems.isEmpty()) {
        std::cout << "\n  [!] Cannot checkout an empty cart.\n";
        return false;
    }

    std::vector<Product> purchasedItems = cartItems.toVector();
    double grandTotal = cartItems.getTotalAmount();

    // 1. Enqueue the order into the global FIFO OrderQueue for Manager Processing
    int assignedOrderID = OrderQueue::getInstance().enqueueOrder(customerName, purchasedItems, grandTotal);

    // 2. Generate the Customer Receipt text file
    std::string fileName = "Receipt_" + customerName + ".txt";
    try {
        std::ofstream receiptFile(fileName);
        if (!receiptFile.is_open()) {
            throw std::runtime_error("Error: Could not generate receipt file.");
        }

        receiptFile << "=================================================\n";
        receiptFile << "             EZBAZAR & SMARTCART ENGINE          \n";
        receiptFile << "                  OFFICIAL RECEIPT               \n";
        receiptFile << "=================================================\n";
        receiptFile << "Order ID : #" << assignedOrderID << " (Queued for Dispatch)\n";
        receiptFile << "Customer : " << customerName << "\n\n";
        receiptFile << "Items Purchased:\n";
        receiptFile << "-------------------------------------------------\n";

        for (size_t i = 0; i < purchasedItems.size(); ++i) {
            double sub = purchasedItems[i].getPrice() * purchasedItems[i].getCartQuantity();
            receiptFile << "- [" << purchasedItems[i].getProductID() << "] "
                        << purchasedItems[i].getName()
                        << " | Rs. " << std::fixed << std::setprecision(2) << purchasedItems[i].getPrice()
                        << " x " << purchasedItems[i].getCartQuantity()
                        << " = Rs. " << sub << "\n";
        }

        receiptFile << "-------------------------------------------------\n";
        receiptFile << "Grand Total: Rs. " << std::fixed << std::setprecision(2) << grandTotal << "\n";
        receiptFile << "=================================================\n";
        receiptFile << "Thank you for shopping at EzBazar!\n";
        receiptFile.close();

        std::cout << "\n  [SUCCESS] Checkout complete!\n";
        std::cout << "  -> Order #" << assignedOrderID << " added to the Manager's FIFO OrderQueue.\n";
        std::cout << "  -> Receipt saved to '" << fileName << "'.\n";

        cartItems.clear();
        return true;

    } catch (const std::exception& e) {
        std::cerr << "  [Exception] " << e.what() << "\n";
        return false;
    }
}
