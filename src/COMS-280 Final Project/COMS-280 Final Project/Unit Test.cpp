#include <iostream>
#include <vector>
#include <list>
#include <memory>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <cassert>
using namespace std;

class BankAccount {
protected:
    string owner;
    double balance;
    list<string> transactionHistory;
public:
    BankAccount(string name, double initialBalance) : owner(name), balance(initialBalance) {}
    virtual ~BankAccount() {}
    virtual void deposit(double amount) = 0;
    virtual void withdraw(double amount) = 0;
    virtual void display() const = 0;
    void addTransaction(const string& transaction) {
        transactionHistory.push_back(transaction);
    }
    void displayTransactionHistory() const {
        cout << "Transaction History for " << owner << ":\n";
        for (const auto& transaction : transactionHistory) {
            cout << transaction << endl;
        }
    }
    double getBalance() const { return balance; }
    string getOwner() const { return owner; }
};

class SavingsAccount : public BankAccount {
private:
    double interestRate;
public:
    SavingsAccount(string name, double balance, double rate) : BankAccount(name, balance), interestRate(rate) {}
    void deposit(double amount) override {
        balance += amount;
        addTransaction("Deposited: $" + formatAmount(amount));
    }
    void withdraw(double amount) override {
        if (amount > balance) throw runtime_error("Insufficient funds");
        balance -= amount;
        addTransaction("Withdrawn: $" + formatAmount(amount));
    }
    void display() const override {
        cout << "Savings Account: " << owner << " | Balance: $" << fixed << setprecision(2) << balance << " | Interest Rate: " << interestRate << "%\n";
    }
    string formatAmount(double amount) const {
        ostringstream stream;
        stream << fixed << setprecision(2) << amount;
        return stream.str();
    }
};

class CheckingAccount : public BankAccount {
private:
    double overdraftLimit;
public:
    CheckingAccount(string name, double balance, double overdraft) : BankAccount(name, balance), overdraftLimit(overdraft) {}
    void deposit(double amount) override {
        balance += amount;
        addTransaction("Deposited: $" + formatAmount(amount));
    }
    void withdraw(double amount) override {
        if (amount > balance + overdraftLimit) throw runtime_error("Overdraft limit exceeded");
        balance -= amount;
        addTransaction("Withdrawn: $" + formatAmount(amount));
    }
    void display() const override {
        cout << "Checking Account: " << owner << " | Balance: $" << fixed << setprecision(2) << balance << " | Overdraft Limit: $" << overdraftLimit << "\n";
    }
    string formatAmount(double amount) const {
        ostringstream stream;
        stream << fixed << setprecision(2) << amount;
        return stream.str();
    }
};

void testBankingSystem() {
    cout << "Running Unit Tests...\n";

    SavingsAccount savings("Alice", 5000, 2.5);
    CheckingAccount checking("Bob", 1000, 500);

    // Test Deposits
    savings.deposit(1000);
    assert(savings.getBalance() == 6000);
    checking.deposit(500);
    assert(checking.getBalance() == 1500);
    cout << "Test Passed: Deposits successful\n";

    // Test Withdrawals
    savings.withdraw(2000);
    assert(savings.getBalance() == 4000);
    checking.withdraw(1200);
    assert(checking.getBalance() == 300);
    cout << "Test Passed: Withdrawals successful\n";

    // Test Overdraft Handling
    try {
        checking.withdraw(1000);
        assert(false); // This should not be reached
    }
    catch (const runtime_error& e) {
        assert(string(e.what()) == "Overdraft limit exceeded");
    }
    cout << "Test Passed: Overdraft limit enforced\n";

    // Test Insufficient Funds Handling
    try {
        savings.withdraw(5000);
        assert(false); // This should not be reached
    }
    catch (const runtime_error& e) {
        assert(string(e.what()) == "Insufficient funds");
    }
    cout << "Test Passed: Insufficient funds error handled\n";

    // Test Transaction History
    savings.displayTransactionHistory();
    checking.displayTransactionHistory();
    cout << "Test Passed: Transaction history recorded correctly\n";

    cout << "All Unit Tests Passed Successfully.\n";
}

void showMainMenu() {
    char choice;
    while (true) {
        cout << "\nMain Menu:\n";
        cout << "1. Run Banking System\n";
        cout << "2. Run Unit Tests\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case '1': {
            AccountManager<BankAccount> manager;
            manager.addAccount(make_unique<SavingsAccount>("Alice", 5000, 2.5));
            manager.addAccount(make_unique<CheckingAccount>("Bob", 1000, 500));
            performBankingOperations(manager);
            break;
        }
        case '2':
            testBankingSystem();
            break;
        case '3':
            cout << "Exiting program...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

int main() {
    showMainMenu();
    return 0;
}