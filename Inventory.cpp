#include "Inventory.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>

// Helper to convert string to lowercase for case-insensitive search
static std::string toLowerStr(std::string s) {
    for (size_t i = 0; i < s.size(); ++i) {
        s[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
    }
    return s;
}

void Inventory::addProduct(const Product& p) {
    stockTree.insert(p);
}

bool Inventory::getProduct(int id, Product& outProduct) const {
    return stockTree.search(id, outProduct);
}

bool Inventory::removeProduct(int id) {
    return stockTree.remove(id);
}

bool Inventory::productExists(int id) const {
    Product dummy;
    return stockTree.search(id, dummy);
}

// Safely deducts stock in-place inside the AVL node in O(log N) time!
bool Inventory::reduceStock(int id, int amount) {
    if (amount <= 0) return false;
    Product* prodPtr = stockTree.findProductPtr(id);
    if (!prodPtr) return false;

    if (prodPtr->getStockQuantity() >= amount) {
        prodPtr->setStockQuantity(prodPtr->getStockQuantity() - amount);
        return true;
    } else {
        std::cout << "\n  [!] Not enough stock! Only " << prodPtr->getStockQuantity() << " unit(s) left.\n";
        return false;
    }
}

// Restores stock in-place (used when customer removes item from cart)
void Inventory::increaseStock(int id, int amount) {
    if (amount <= 0) return;
    Product* prodPtr = stockTree.findProductPtr(id);
    if (prodPtr) {
        prodPtr->setStockQuantity(prodPtr->getStockQuantity() + amount);
    }
}

// Directly sets stock quantity in O(log N) time
bool Inventory::changeStock(int id, int newStock) {
    if (newStock < 0) return false;
    Product* prodPtr = stockTree.findProductPtr(id);
    if (!prodPtr) return false;
    prodPtr->setStockQuantity(newStock);
    return true;
}

// Displays all products sorted by Product ID (via In-Order AVL Traversal)
void Inventory::displayInventory() const {
    if (stockTree.isEmpty()) {
        std::cout << "\n  [!] The store inventory is currently empty.\n";
        return;
    }

    std::vector<Product> allProducts = stockTree.getAllProductsSorted();
    std::cout << "\n==============================================================================\n";
    std::cout << "      EZBAZAR MASTER INVENTORY (AVL Tree In-Order Traversal | Height: "
              << stockTree.getTreeHeight() << ")\n";
    std::cout << "==============================================================================\n";
    for (size_t i = 0; i < allProducts.size(); ++i) {
        allProducts[i].displayProduct();
    }
    std::cout << "==============================================================================\n";
}

// Filters products by category name
void Inventory::displayByCategory(const std::string& category) const {
    std::vector<Product> allProducts = stockTree.getAllProductsSorted();
    std::string target = toLowerStr(category);
    bool foundAny = false;

    std::cout << "\n--- Category Filter: " << category << " ---\n";
    for (size_t i = 0; i < allProducts.size(); ++i) {
        if (toLowerStr(allProducts[i].getCategory()) == target) {
            allProducts[i].displayProduct();
            foundAny = true;
        }
    }
    if (!foundAny) {
        std::cout << "  No products found in category '" << category << "'.\n";
    }
}

// Searches products by partial keyword in name
void Inventory::searchByNameSubstring(const std::string& keyword) const {
    std::vector<Product> allProducts = stockTree.getAllProductsSorted();
    std::string target = toLowerStr(keyword);
    bool foundAny = false;

    std::cout << "\n--- Search Results for '" << keyword << "' ---\n";
    for (size_t i = 0; i < allProducts.size(); ++i) {
        if (toLowerStr(allProducts[i].getName()).find(target) != std::string::npos) {
            allProducts[i].displayProduct();
            foundAny = true;
        }
    }
    if (!foundAny) {
        std::cout << "  No matching products found.\n";
    }
}

// Seeds default inventory so the app works out-of-the-box
void Inventory::seedDefaultData() {
    addProduct(Product("Groceries",   101, "Basmati_Rice_5kg",     1850.00, 25));
    addProduct(Product("Groceries",   102, "Cooking_Oil_3L",       1620.00, 30));
    addProduct(Product("Dairy",       103, "Fresh_Milk_1L",         220.00, 50));
    addProduct(Product("Bakery",      104, "Whole_Wheat_Bread",     160.00, 40));
    addProduct(Product("Dairy",       105, "Farm_Eggs_Dozen",       340.00, 35));
    addProduct(Product("Electronics", 201, "Wireless_Mouse",       2400.00, 15));
    addProduct(Product("Electronics", 202, "Mechanical_Keyboard",  6500.00, 10));
    addProduct(Product("Electronics", 203, "USB_C_Fast_Charger",   1950.00, 20));
    addProduct(Product("Beverages",   301, "Green_Tea_Pack",        450.00, 40));
    addProduct(Product("Snacks",      302, "Dark_Chocolate_Bar",    380.00, 45));
}

// Loads inventory from file; if missing or empty, seeds default store data
void Inventory::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "  [Notice] No existing '" << filename << "' found. Seeding default catalog...\n";
        seedDefaultData();
        saveToFile(filename);
        return;
    }

    int loadedCount = 0;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string cat, idStr, name, priceStr, stockStr;

        if (!std::getline(ss, cat, '|')) continue;
        if (!std::getline(ss, idStr, '|')) continue;
        if (!std::getline(ss, name, '|')) continue;
        if (!std::getline(ss, priceStr, '|')) continue;
        if (!std::getline(ss, stockStr, '|')) continue;

        try {
            int id = std::stoi(idStr);
            double price = std::stod(priceStr);
            int qty = std::stoi(stockStr);
            addProduct(Product(cat, id, name, price, qty));
            loadedCount++;
        } catch (...) {
            continue; // Skip malformed lines gracefully
        }
    }
    file.close();

    if (loadedCount == 0) {
        seedDefaultData();
        saveToFile(filename);
    }
}

// Saves all AVL Tree nodes to file in sorted ID order
void Inventory::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "  [Error] Could not save inventory to " << filename << "\n";
        return;
    }

    std::vector<Product> allProducts = stockTree.getAllProductsSorted();
    for (size_t i = 0; i < allProducts.size(); ++i) {
        const Product& prod = allProducts[i];
        file << prod.getCategory() << "|"
             << prod.getProductID() << "|"
             << prod.getName() << "|"
             << prod.getPrice() << "|"
             << prod.getStockQuantity() << "\n";
    }
    file.close();
}
