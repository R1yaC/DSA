#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <cmath>
using namespace std;

const int TABLE_SIZE = 10007;

struct Transaction {
    int transactionID;
    int customerID;
    double time;
    double amount;
    int fraud;

    // Comparator for sorting by amount
    bool operator < (const Transaction& other) const {
        return amount < other.amount;
    }
};

class HashTable {
private:
    vector<list<int>> table;

public:
    HashTable() : table(TABLE_SIZE) {}

    int hashFunction(int key) {
        return key % TABLE_SIZE;
    }

    void insert(int transactionID) {
        int index = hashFunction(transactionID);
        for (auto id : table[index]) {
            if (id == transactionID) return;
        }
        table[index].push_back(transactionID);
    }

    bool search(int transactionID) {
        int index = hashFunction(transactionID);
        for (int id : table[index]) {
            if (id == transactionID) {
                return true;
            }
        }
        return false;
    }
};

class TransactionArray {
private:
    vector<Transaction> transactions;
    int totalTransactions = 0;
    int fraudTransactions = 0;

public:
    void addTransaction(int transactionID, double time, double amount, int fraud) {
        int customerID = static_cast<int>(time * 1000 + amount) % 999999;
        transactions.push_back({transactionID, customerID, time, amount, fraud});
        totalTransactions++;
        if (fraud) fraudTransactions++;
    }

    double getFraudPercentage() const {
        if (totalTransactions == 0) return 0.0;
        float twoday_percent = ((double)fraudTransactions / totalTransactions) * 100;
        return twoday_percent / 2;
    }

    void displayTransactions(int fraudStatus) {
        cout << "Transactions (" << (fraudStatus ? "Fraud" : "Non-Fraud") << "):" << endl;
        vector<Transaction> sortedTransactions;

        for (const Transaction& transaction : transactions) {
            if (transaction.fraud == fraudStatus) {
                sortedTransactions.push_back(transaction);
            }
        }

        // Sort transactions by amount in ascending order
        sort(sortedTransactions.begin(), sortedTransactions.end());

        for (const Transaction& transaction : sortedTransactions) {
            cout << "Customer ID: " << transaction.customerID << endl
                << "Transaction ID: " << transaction.transactionID
                << ", Time: " << transaction.time
                << ", Amount: $" << transaction.amount
                << ", Class: " << (transaction.fraud ? "Fraud" : "Non-Fraud") << endl;
        }
    }
    
};

int main() {
    HashTable hashTable;
    TransactionArray transactionArray;
    ifstream file("creditcard.csv");
    string line;
    getline(file, line);

    int transactionID = 0;
    while (getline(file, line)) {
        istringstream s(line);
        string value;
        double time, amount;
        int fraud;

        try {
            getline(s, value, ',');
            time = stod(value);
            for (int i = 0; i < 28; i++) {
                getline(s, value, ',');
            }
            getline(s, value, ',');
            amount = stod(value);
            getline(s, value);
            value.erase(remove(value.begin(), value.end(), '"'), value.end());
            fraud = stoi(value);

            if (!hashTable.search(transactionID)) {
                hashTable.insert(transactionID);
                transactionArray.addTransaction(transactionID, time, amount, fraud);
            }
            transactionID++;
        } catch (const invalid_argument& e) {
            cerr << "Invalid argument: " << e.what() << '\n';
            cerr << "Faulty line: " << line << '\n';
        } catch ( const exception& e) {
            cerr << "Error processing line: " << e.what() << '\n';
            cerr << "Faulty line: " << line << '\n';
        }
    }

    int choice;
    cout << "Enter 1 for fraud transactions or 0 for non-fraud transactions: ";
    cin >> choice;
    transactionArray.displayTransactions(choice);

    int days;
    cout << "Enter the number of days to project the fraud percentage: ";
    cin >> days;
    cout << "Projected fraud percentage for the next " << days << " days: " << transactionArray.getFraudPercentage() * days << "%" << endl;

    return 0;
}
