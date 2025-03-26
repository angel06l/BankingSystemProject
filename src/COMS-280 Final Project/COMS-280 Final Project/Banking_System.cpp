#include <iostream>
#include <list>
#include <memory>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <functional>
using namespace std;

/**
 * Utility class to handle interest calculation for savings accounts.
 */
class InterestCalculator {
public:
    static double calculateInterest(double balance, double rate) {
        return balance * (rate / 100.0);
    }
};

/**
 * Utility class to enforce overdraft rules in checking accounts.
 */
class OverdraftProtection {
public:
    static bool canWithdraw(double balance, double overdraft, double amount) {
        return amount <= (balance + overdraft);
    }
};

/**
 * Interface for interest-bearing accounts.
 * Enables runtime polymorphism based on feature rather than type.
 */
class InterestBearing {
public:
    virtual void applyInterest() = 0;
    virtual ~InterestBearing() = default;
};

/**
 * Base class representing a generic bank account.
 * Implements polymorphic behavior through virtual functions.
 * Provides common interface for all account types.
 */
class BankAccount {
protected:
    string owner;
    double balance;
    list<string> transactionHistory;

    // Helper method to format currency with two decimal places
    string formatAmount(double amount) const {
        ostringstream stream;
        stream << fixed << setprecision(2) << amount;
        return stream.str();
    }

public:
    BankAccount(string name, double initialBalance) : owner(name), balance(initialBalance) {}
    virtual ~BankAccount() = default;

    // Pure virtual methods to be implemented by derived classes
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

/**
 * Derived class representing a savings account.
 * Implements InterestBearing interface to support advanced polymorphism.
 */
class SavingsAccount : public BankAccount, public InterestBearing {
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

    void applyInterest() override {
        double interest = InterestCalculator::calculateInterest(balance, interestRate);
        deposit(interest);
        addTransaction("Interest Applied: $" + formatAmount(interest));
    }

    void display() const override {
        cout << "Savings Account: " << owner << " | Balance: $" << fixed << setprecision(2) << balance
            << " | Interest Rate: " << interestRate << "%\n";
    }
};

/**
 * Derived class representing a checking account.
 * Uses OverdraftProtection utility class for safe withdrawal.
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
        if (!OverdraftProtection::canWithdraw(balance, overdraftLimit, amount))
            throw runtime_error("Overdraft limit exceeded");
        balance -= amount;
        addTransaction("Withdrawn: $" + formatAmount(amount));
    }

    void display() const override {
        cout << "Checking Account: " << owner << " | Balance: $" << fixed << setprecision(2) << balance
            << " | Overdraft Limit: $" << overdraftLimit << "\n";
    }
};

/**
 * Factory class for creating accounts using C++14 features.
 * Demonstrates use of make_unique and simplified control flow.
 */
class AccountFactory {
public:
    static unique_ptr<BankAccount> createAccount(const string& type, const string& name, double balance, double extra = 0.0) {
        if (type == "savings") return make_unique<SavingsAccount>(name, balance, extra);
        if (type == "checking") return make_unique<CheckingAccount>(name, balance, extra);
        throw invalid_argument("Unknown account type");
    }
};

/**
 * Struct representing a node in the customer linked list.
 */
struct CustomerNode {
    unique_ptr<BankAccount> account;
    CustomerNode* next;

    CustomerNode(unique_ptr<BankAccount> acc)
        : account(move(acc)), next(nullptr) {
    }
};

/**
 * Manages a singly linked list of bank accounts.
 */
class CustomerList {
private:
    CustomerNode* head;

public:
    CustomerList() : head(nullptr) {}

    ~CustomerList() {
        while (head) {
            auto* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void addCustomer(unique_ptr<BankAccount> account) {
        auto* newNode = new CustomerNode(move(account));
        newNode->next = head;
        head = newNode;
    }

    bool deleteCustomer(const string& name) {
        auto* curr = head;
        CustomerNode* prev = nullptr;

        while (curr) {
            if (curr->account->getOwner() == name) {
                if (prev) prev->next = curr->next;
                else head = curr->next;
                delete curr;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    BankAccount* getCustomerByName(const string& name) {
        for (auto* curr = head; curr != nullptr; curr = curr->next) {
            if (curr->account->getOwner() == name)
                return curr->account.get();
        }
        return nullptr;
    }

    void displayAll() const {
        for (auto* curr = head; curr != nullptr; curr = curr->next) {
            curr->account->display();
        }
    }
};

/**
 * Interface for interacting with customer accounts using advanced polymorphism.
 */
void performBankingOperations(CustomerList& customers) {
    string name;
    char choice;
    double amount;

    while (true) {
        cout << "\nEnter account owner name (or 'exit' to quit): ";
        cin >> name;
        if (name == "exit") break;

        auto* account = customers.getCustomerByName(name);
        if (!account) {
            cout << "Account not found.\n";
            continue;
        }

        cout << "\nChoose operation: \nD - Deposit\nW - Withdraw\nS - Show Account\nH - Show Transaction History\nI - Apply Interest\nE - Exit\nChoice: ";
        cin >> choice;

        switch (choice) {
        case 'D': case 'd':
            cout << "Enter deposit amount: ";
            cin >> amount;
            account->deposit(amount);
            cout << "Deposit successful.\n";
            break;

        case 'W': case 'w':
            cout << "Enter withdrawal amount: ";
            cin >> amount;
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

        case 'H': case 'h': {
            auto displayTransactions = [](const BankAccount& acc) {
                acc.displayTransactionHistory();
                };
            displayTransactions(*account);
            break;
        }

        case 'I': case 'i': {
            if (auto* ib = dynamic_cast<InterestBearing*>(account)) {
                ib->applyInterest();
                cout << "Interest applied.\n";
            }
            else {
                cout << "This account does not support interest calculation.\n";
            }
            break;
        }

        case 'E': case 'e':
            return;

        default:
            cout << "Invalid choice.\n";
        }
    }
}

/**
 * Entry point: Initializes customers using AccountFactory.
 */
int main() {
    CustomerList customers;
    customers.addCustomer(AccountFactory::createAccount("savings", "Laurie", 5000, 2.5));
    customers.addCustomer(AccountFactory::createAccount("checking", "Larry", 1000, 500));
    customers.addCustomer(AccountFactory::createAccount("savings", "David", 10000, 2.5));
    customers.addCustomer(AccountFactory::createAccount("checking", "Luis", 2000, 500));

    performBankingOperations(customers);
    return 0;
}