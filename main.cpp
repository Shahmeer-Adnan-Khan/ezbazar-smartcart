#include <iostream>
#include <string>
#include <limits>
#include "Product.h"
#include "User.h"
#include "SmartCart.h"
#include "Inventory.h"
#include "OrderQueue.h"
#include "UndoStack.h"
#include "Utils.h"

using namespace std;

// Helper to safely read an integer within a range [minVal, maxVal]
bool readIntInRange(int& outVal, int minVal, int maxVal) {
    if (!(cin >> outVal) || outVal < minVal || outVal > maxVal) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return false;
    }
    return true;
}

int main() {
    // 1. Initialize Singleton Inventory & Load Data into the AVL Tree
    Inventory& storeInventory = Inventory::getInstance();
    storeInventory.loadFromFile("inventory.txt");

    // 2. Initialize Manager Undo Stack (persists across manager sessions)
    UndoStack managerUndoStack;

    int mainChoice = 0;

    // 3. Master Application Loop
    while (mainChoice != 3) {
        clearScreen();
        cout << "======================================================\n";
        cout << "    Welcome to EzBazar & SmartCart Engine (DSA Ed.)   \n";
        cout << "======================================================\n";
        cout << "  1. Store Manager Login\n";
        cout << "  2. Customer (Start Shopping)\n";
        cout << "  3. Exit Application\n";
        cout << "======================================================\n";
        cout << "Choose an option (1-3): ";

        if (!readIntInRange(mainChoice, 1, 3)) {
            cout << "\n  [!] Invalid input! Please enter 1, 2, or 3.\n";
            cout << "Press Enter to continue...";
            cin.get();
            continue;
        }

        // FLOW 1: STORE MANAGER DASHBOARD
        if (mainChoice == 1) {
            Manager admin("Abu Huraira Ahmad");

            clearScreen();
            if (!admin.loginManager()) {
                cout << "\n  [!] Access Denied! Press Enter to return.";
                cin.get();
                continue;
            }

            cout << "Loading Manager Dashboard...\n";
            waitSeconds(0.8f);

            int mgrChoice = 0;
            while (mgrChoice != 7) {
                clearScreen();
                admin.displayDashboard();
                cout << "Choose an action (1-7): ";

                if (!readIntInRange(mgrChoice, 1, 7)) {
                    cout << "\n  [!] Invalid input! Please enter a number between 1 and 7.\n";
                    cout << "Press Enter to continue...";
                    cin.get();
                    continue;
                }

                // Option 1: View Master Inventory (AVL Tree In-Order Traversal)
                if (mgrChoice == 1) {
                    clearScreen();
                    storeInventory.displayInventory();
                    pauseConsole();
                }
                // Option 2: Add a New Product (AVL Insert + Push UndoStack)
                else if (mgrChoice == 2) {
                    clearScreen();
                    cout << "--- ADD NEW PRODUCT (AVL Tree Insert) ---\n";
                    int id, stock;
                    double price;
                    string cat, name;

                    cout << "Product ID (Positive Number): ";
                    if (!(cin >> id) || id <= 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "\n  [!] Invalid Product ID.\n";
                        cout << "Press Enter to return...";
                        cin.get();
                        continue;
                    }

                    if (storeInventory.productExists(id)) {
                        cout << "\n  [!] Error: A product with ID " << id << " already exists!\n";
                        pauseConsole();
                        continue;
                    }

                    cout << "Category (e.g., Groceries, Dairy, Electronics): ";
                    cin >> cat;
                    cout << "Product Name (Use_Underscores_For_Spaces): ";
                    cin >> name;
                    cout << "Price (Rs.): ";
                    if (!(cin >> price) || price < 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "\n  [!] Invalid price.\n";
                        cout << "Press Enter to return...";
                        cin.get();
                        continue;
                    }
                    cout << "Stock Quantity: ";
                    if (!(cin >> stock) || stock < 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "\n  [!] Invalid stock quantity.\n";
                        cout << "Press Enter to return...";
                        cin.get();
                        continue;
                    }

                    Product newProd(cat, id, name, price, stock);
                    storeInventory.addProduct(newProd);
                    storeInventory.saveToFile("inventory.txt");

                    // Record on LIFO UndoStack so manager can undo this addition
                    managerUndoStack.push(Action(ACTION_ADD_PRODUCT, newProd));

                    cout << "\n  [SUCCESS] Product added to AVL Tree & recorded in UndoStack!\n";
                    pauseConsole();
                }
                // Option 3: Remove a Product (AVL Remove + Push UndoStack)
                else if (mgrChoice == 3) {
                    clearScreen();
                    cout << "--- REMOVE A PRODUCT (AVL Tree Remove) ---\n";
                    storeInventory.displayInventory();
                    cout << "\nEnter the Product ID to remove (or 0 to cancel): ";
                    int removeID;
                    if (!(cin >> removeID) || removeID == 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }

                    Product backupCopy;
                    if (storeInventory.getProduct(removeID, backupCopy) &&
                        storeInventory.removeProduct(removeID)) {
                        storeInventory.saveToFile("inventory.txt");
                        managerUndoStack.push(Action(ACTION_REMOVE_PRODUCT, backupCopy));
                        cout << "\n  [SUCCESS] Product [" << removeID << "] removed & backed up in UndoStack!\n";
                    } else {
                        cout << "\n  [!] Error: Product ID not found.\n";
                    }
                    pauseConsole();
                }
                // Option 4: Change Stock Quantity (AVL In-Place Update + Push UndoStack)
                else if (mgrChoice == 4) {
                    clearScreen();
                    cout << "--- CHANGE STOCK QUANTITY (O(log N) AVL Update) ---\n";
                    storeInventory.displayInventory();
                    cout << "\nEnter the Product ID to update: ";
                    int updateID;
                    if (!(cin >> updateID)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }

                    Product currentProd;
                    if (!storeInventory.getProduct(updateID, currentProd)) {
                        cout << "\n  [!] Error: Product ID not found.\n";
                        pauseConsole();
                        continue;
                    }

                    cout << "Current Stock for '" << currentProd.getName() << "': "
                         << currentProd.getStockQuantity() << "\n";
                    cout << "Enter new stock quantity: ";
                    int newStock;
                    if (!(cin >> newStock) || newStock < 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "\n  [!] Error: Stock quantity cannot be negative.\n";
                        cout << "Press Enter to return...";
                        cin.get();
                        continue;
                    }

                    int oldStock = currentProd.getStockQuantity();
                    storeInventory.changeStock(updateID, newStock);
                    storeInventory.saveToFile("inventory.txt");

                    managerUndoStack.push(Action(ACTION_CHANGE_STOCK, currentProd, oldStock, newStock));
                    cout << "\n  [SUCCESS] Stock updated from " << oldStock << " to " << newStock
                         << " (Push recorded on UndoStack)!\n";
                    pauseConsole();
                }
                // Option 5: Undo Last Manager Action (LIFO UndoStack Pop)
                else if (mgrChoice == 5) {
                    clearScreen();
                    cout << "--- UNDO LAST MANAGER ACTION (LIFO Stack) ---\n";
                    Action lastAction(ACTION_ADD_PRODUCT, Product());
                    if (!managerUndoStack.pop(lastAction)) {
                        cout << "\n  [!] UndoStack is empty! No manager actions to undo.\n";
                        pauseConsole();
                        continue;
                    }

                    if (lastAction.type == ACTION_ADD_PRODUCT) {
                        int id = lastAction.productSnapshot.getProductID();
                        storeInventory.removeProduct(id);
                        cout << "\n  [UNDO SUCCESS] Reversed 'Add Product': Removed [" << id << "] "
                             << lastAction.productSnapshot.getName() << " from AVL Tree.\n";
                    } else if (lastAction.type == ACTION_REMOVE_PRODUCT) {
                        storeInventory.addProduct(lastAction.productSnapshot);
                        cout << "\n  [UNDO SUCCESS] Reversed 'Remove Product': Restored ["
                             << lastAction.productSnapshot.getProductID() << "] "
                             << lastAction.productSnapshot.getName() << " back into AVL Tree.\n";
                    } else if (lastAction.type == ACTION_CHANGE_STOCK) {
                        int id = lastAction.productSnapshot.getProductID();
                        storeInventory.changeStock(id, lastAction.oldStock);
                        cout << "\n  [UNDO SUCCESS] Reversed 'Stock Change' for [" << id << "] "
                             << lastAction.productSnapshot.getName() << ": Restored stock from "
                             << lastAction.newStock << " back to " << lastAction.oldStock << ".\n";
                    }

                    storeInventory.saveToFile("inventory.txt");
                    pauseConsole();
                }
                // Option 6: Process Pending Customer Orders (FIFO OrderQueue)
                else if (mgrChoice == 6) {
                    clearScreen();
                    OrderQueue& q = OrderQueue::getInstance();
                    q.displayPendingOrders();

                    if (!q.isEmpty()) {
                        cout << "\nDequeue and approve the front order now? (y/n): ";
                        char ch;
                        cin >> ch;
                        if (ch == 'y' || ch == 'Y') {
                            Order processedOrder;
                            if (q.dequeue(processedOrder)) {
                                cout << "\n  [ORDER APPROVED & DISPATCHED]\n";
                                cout << "  -> Order ID : #" << processedOrder.orderID << "\n";
                                cout << "  -> Customer : " << processedOrder.customerName << "\n";
                                cout << "  -> Total    : Rs. " << processedOrder.totalAmount << "\n";
                            }
                        }
                    }
                    pauseConsole();
                }
            }
        }

        // FLOW 2: CUSTOMER SHOPPING & SMARTCART
        else if (mainChoice == 2) {
            clearScreen();
            string custName;
            cout << "Enter your first name for the receipt: ";
            cin >> custName;

            Customer shopper(custName);
            SmartCart myCart;
            int custChoice = 0;

            while (custChoice != 6) {
                clearScreen();
                shopper.displayDashboard();
                cout << "Choose an action (1-6): ";

                if (!readIntInRange(custChoice, 1, 6)) {
                    cout << "\n  [!] Invalid input! Please enter a number between 1 and 6.\n";
                    cout << "Press Enter to continue...";
                    cin.get();
                    continue;
                }

                // Option 1: Browse All Products & Add to SmartCart
                if (custChoice == 1) {
                    while (true) {
                        clearScreen();
                        storeInventory.displayInventory();

                        cout << "\nEnter Product ID to add to SmartCart (or 0 to return): ";
                        int pickID;
                        if (!(cin >> pickID) || pickID == 0) {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            break;
                        }

                        if (!storeInventory.productExists(pickID)) {
                            cout << "\n  [!] Invalid Product ID. Please try again.\n";
                            pauseConsole();
                            continue;
                        }

                        cout << "How many units would you like to buy? ";
                        int qty;
                        if (!(cin >> qty) || qty <= 0) {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cout << "\n  [!] Quantity must be a positive number.\n";
                            cout << "Press Enter to continue...";
                            cin.get();
                            continue;
                        }

                        if (storeInventory.reduceStock(pickID, qty)) {
                            Product foundProduct;
                            storeInventory.getProduct(pickID, foundProduct);
                            myCart.addItem(foundProduct, qty);
                            cout << "\n  [ADDED] " << qty << "x " << foundProduct.getName()
                                 << " added to your SmartCart (Doubly Linked List)!\n";
                        }

                        cout << "\nAdd another item? (y/n): ";
                        char decision;
                        cin >> decision;
                        if (decision == 'y' || decision == 'Y') {
                            continue;
                        } else {
                            break;
                        }
                    }
                }
                // Option 2: Filter by Category or Search by Name
                else if (custChoice == 2) {
                    clearScreen();
                    cout << "--- SEARCH & FILTER CATALOG ---\n";
                    cout << "1. Filter by Category Name (e.g., Groceries, Dairy, Electronics)\n";
                    cout << "2. Search by Product Name Keyword\n";
                    cout << "Choose (1-2): ";
                    int subChoice;
                    if (readIntInRange(subChoice, 1, 2)) {
                        if (subChoice == 1) {
                            string cat;
                            cout << "Enter Category: ";
                            cin >> cat;
                            storeInventory.displayByCategory(cat);
                        } else {
                            string kw;
                            cout << "Enter Keyword: ";
                            cin >> kw;
                            storeInventory.searchByNameSubstring(kw);
                        }
                    }
                    pauseConsole();
                }
                // Option 3: View SmartCart (Doubly Linked List)
                else if (custChoice == 3) {
                    clearScreen();
                    myCart.viewCart();
                    pauseConsole();
                }
                // Option 4: Remove an Item from SmartCart (O(1) Pointer Relinking + Stock Restore)
                else if (custChoice == 4) {
                    clearScreen();
                    myCart.viewCart();
                    if (myCart.isEmpty()) {
                        pauseConsole();
                        continue;
                    }

                    cout << "\nEnter Product ID to remove from your cart (or 0 to cancel): ";
                    int remID;
                    if (!(cin >> remID) || remID == 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }

                    Product removedItem;
                    if (myCart.removeItem(remID, removedItem)) {
                        // Return the reserved quantity back to the AVL Inventory!
                        storeInventory.increaseStock(remID, removedItem.getCartQuantity());
                        cout << "\n  [REMOVED] " << removedItem.getName() << " (Qty: "
                             << removedItem.getCartQuantity()
                             << ") removed from SmartCart & returned to Inventory stock!\n";
                    } else {
                        cout << "\n  [!] That Product ID is not in your cart.\n";
                    }
                    pauseConsole();
                }
                // Option 5: Checkout & Pay
                else if (custChoice == 5) {
                    clearScreen();
                    myCart.viewCart();
                    if (myCart.checkout(shopper.getUsername())) {
                        storeInventory.saveToFile("inventory.txt");
                        pauseConsole("\nPress Enter to return to main menu...");
                        break;
                    }
                    pauseConsole();
                }
                // Option 6: Leave Store (Restore any unpurchased cart items back to stock!)
                else if (custChoice == 6) {
                    if (!myCart.isEmpty()) {
                        vector<Product> leftover = myCart.getItemsVector();
                        for (size_t i = 0; i < leftover.size(); ++i) {
                            storeInventory.increaseStock(leftover[i].getProductID(),
                                                         leftover[i].getCartQuantity());
                        }
                        cout << "\n  [Notice] Unpurchased cart items have been returned to store shelves.\n";
                        waitSeconds(0.8f);
                    }
                }
            }
        }
    }

    clearScreen();
    cout << "System shutting down. Saving final AVL inventory state...\n";
    storeInventory.saveToFile("inventory.txt");
    cout << "Goodbye!\n";
    return 0;
}
