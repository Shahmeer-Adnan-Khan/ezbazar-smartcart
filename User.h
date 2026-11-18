#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>

class User {
protected:
    std::string username;
    std::string role;
    std::string password;

public:
    User(std::string uname, std::string r);
    virtual ~User() {}

    std::string getUsername() const;
    std::string getRole() const;
    bool loginManager();

    // Pure virtual function demonstrating Runtime Polymorphism
    virtual void displayDashboard() const = 0;
};

class Manager : public User {
public:
    Manager(std::string uname);
    void displayDashboard() const override;
};

class Customer : public User {
public:
    Customer(std::string uname);
    void displayDashboard() const override;
};

#endif 
