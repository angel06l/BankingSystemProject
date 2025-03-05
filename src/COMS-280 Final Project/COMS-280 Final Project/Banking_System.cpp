#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <stdexcept>
#include <iomanip>
#include <sstream>
using namespace std;

/**
 * Base class representing a generic bank account.
 * Provides common functionality for all account types.
 * Implements polymorphic behavior through virtual functions.
 */
class BankAccount {
protected:
    string owner;
    double balance;
    list<string> transactionHistory;

    /**
     * Formats a monetary amount to two decimal places.
     */
    string formatAmount(double amount) const {
        ostringstream stream;
        stream << fixed << setprecision(2) << amount;
        return stream.str();
    }

public:
    BankAccount(string name, double initialBalance) : owner(name), balance(initialBalance) {}
    virtual ~BankAccount() = default; // Ensures proper cleanup in derived classes

    // Pure virtual functions for deposit, withdrawal, and display (polymorphism)
    virtual void deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0;
    virtual void display() const = 0;

    /**
     * Adds a transaction to the account's history.
     */
    void addTransaction(const string& transaction) {
        transactionHistory.push_back(transaction);
    }

    /**
     * Displays the transaction history of the account.
     */
    void displayTransactionHistory() const {
        cout << "Transaction History for " << owner << ":\n";
        for (const auto& transaction : transactionHistory) {
            cout << transaction << endl;
        }
    }

    // Getters for account balance and owner name
    double getBalance() const { return balance; }
    string getOwner() const { return owner; }
};

/**
 * Derived class representing a savings account.
 * Implements specific deposit and withdrawal behaviors.
 */
class SavingsAccount : public BankAccount {
private:
    double interestRate;

public:
    SavingsAccount(string name, double balance, double rate)
        : BankAccount(name, balance), interestRate(rate) {
    }

    void deposit(double amount) override {
        balance += amount;
        addTransaction("Deposited: $" + formatAmount(amount));
    }

    void withdraw(double amount) override {
        if (amount > balance)
            throw runtime_error("Insufficient funds");
        balance -= amount;
        addTransaction("Withdrawn: $" + formatAmount(amount));
    }

    void display() const override {
        cout << "Savings Account: " << owner << " | Balance: $" << fixed
            << setprecision(2) << balance << " | Interest Rate: " << interestRate << "%\n";
    }
};

/**
 * Derived class representing a checking account.
 * Implements specific withdrawal behavior with overdraft limit.
 */
class CheckingAccount : public BankAccount {
private:
    double overdraftLimit;

public:
    CheckingAccount(string name, double balance, double overdraft)
        : BankAccount(name, balance), overdraftLimit(overdraft) {
    }

    void deposit(double amount) override {
        balance += amount;
        addTransaction("Deposited: $" + formatAmount(amount));
    }

    void withdraw(double amount) override {
        if (amount > balance + overdraftLimit)
            throw runtime_error("Overdraft limit exceeded");
        balance -= amount;
        addTransaction("Withdrawn: $" + formatAmount(amount));
    }

    void display() const override {
        cout << "Checking Account: " << owner << " | Balance: $" << fixed
            << setprecision(2) << balance << " | Overdraft Limit: $" << overdraftLimit << "\n";
    }
};

/**
 * Manages multiple bank accounts, enabling polymorphic behavior.
 */
class AccountManager {
private:
    vector<unique_ptr<BankAccount>> accounts;

public:
    /**
     * Adds an account to the manager.
     */
    void addAccount(unique_ptr<BankAccount> account) {
        accounts.push_back(move(account));
    }

    /**
     * Displays all accounts managed.
     */
    void displayAccounts() const {
        for (const auto& acc : accounts) {
            acc->display();
        }
    }

    /**
     * Retrieves an account by the owner's name.
     */
    BankAccount* getAccount(const string& name) {
        for (const auto& acc : accounts) {
            if (acc->getOwner() == name) {
                return acc.get();
            }
        }
        return nullptr;
    }
};

/**
 * Handles user interactions for banking operations.
 */
void performBankingOperations(AccountManager& manager) {
    string name;
    char choice;
    double amount;

    while (true) {
        cout << "\nEnter account owner name (or 'exit' to quit): ";
        cin >> name;
        if (name == "exit") break;

        BankAccount* account = manager.getAccount(name);
        if (!account) {
            cout << "Account not found.\n";
            continue;
        }

        cout << "\nChoose operation: \nD - Deposit\nW - Withdraw\nS - Show Account\nH - Show Transaction History\nE - Exit\nChoice: ";
        cin >> choice;

        switch (choice) {
        case 'D': case 'd':
            cout << "Enter deposit amount: ";
            cin >> amount;
            if (amount <= 0) {
                cout << "Invalid deposit amount.\n";
                continue;
            }
            account->deposit(amount);
            cout << "Deposit successful.\n";
            break;

        case 'W': case 'w':
            cout << "Enter withdrawal amount: ";
            cin >> amount;
            if (amount <= 0) {
                cout << "Invalid withdrawal amount.\n";
                continue;
            }
            try {
                account->withdraw(amount);
                cout << "Withdrawal successful.\n";
            }
            catch (const exception& e) {
                cout << "Error: " << e.what() << endl;
            }
            break;

        case 'S': case 's':
            account->display();
            break;

        case 'H': case 'h':
            account->displayTransactionHistory();
            break;

        case 'E': case 'e':
            return;

        default:
            cout << "Invalid choice.\n";
        }
    }
}

int main() {
    AccountManager manager;
    manager.addAccount(make_unique<SavingsAccount>("Laurie", 5000, 2.5));
    manager.addAccount(make_unique<CheckingAccount>("Larry", 1000, 500));
    manager.addAccount(make_unique<SavingsAccount>("David", 10000, 2.5));
    manager.addAccount(make_unique<CheckingAccount>("Luis", 2000, 500));

    performBankingOperations(manager);
    return 0;
}
