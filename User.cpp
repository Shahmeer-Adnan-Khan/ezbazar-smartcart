#include "User.h"
#include "OrderQueue.h"
#include <limits>

User::User(std::string uname, std::string r) : username(uname), role(r) {}

std::string User::getUsername() const { return username; }
std::string User::getRole() const { return role; }

Manager::Manager(std::string uname) : User(uname, "Manager") {}

void Manager::displayDashboard() const {
    std::cout << "\n======================================================\n";
    std::cout << "        MANAGER DASHBOARD (" << username << ")        \n";
    std::cout << "======================================================\n";
    std::cout << "  1. View Master Inventory (AVL Tree In-Order)\n";
    std::cout << "  2. Add a New Product (AVL Insert + Push UndoStack)\n";
    std::cout << "  3. Remove a Product (AVL Remove + Push UndoStack)\n";
    std::cout << "  4. Change Stock Quantity (AVL Update + Push UndoStack)\n";
    std::cout << "  5. Undo Last Manager Action (LIFO UndoStack Pop)\n";
    std::cout << "  6. Process Pending Customer Orders (FIFO OrderQueue: "
              << OrderQueue::getInstance().getSize() << " waiting)\n";
    std::cout << "  7. Logout to Main Menu\n";
    std::cout << "======================================================\n";
}

Customer::Customer(std::string uname) : User(uname, "Customer") {}

void Customer::displayDashboard() const {
    std::cout << "\n======================================================\n";
    std::cout << "        EZBAZAR CUSTOMER AISLE (" << username << ")   \n";
    std::cout << "======================================================\n";
    std::cout << "  1. Browse All Products & Add to SmartCart\n";
    std::cout << "  2. Filter by Category or Search by Name\n";
    std::cout << "  3. View My SmartCart (Doubly Linked List)\n";
    std::cout << "  4. Remove an Item from My SmartCart\n";
    std::cout << "  5. Checkout & Pay (Enqueue Order + Save Receipt)\n";
    std::cout << "  6. Leave Store\n";
    std::cout << "======================================================\n";
}

bool User::loginManager() {
    password = "Manager123";
    std::string guessedPassword;
    int attempts = 3;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while (attempts > 0) {
        std::cout << "Enter Manager Password (hint: Manager123): ";
        std::getline(std::cin, guessedPassword);

        if (guessedPassword == password) {
            std::cout << "Login successful!\n";
            return true;
        } else {
            attempts--;
            std::cout << "Wrong password! Attempts left: " << attempts << "\n";
        }
    }
    return false;
}
