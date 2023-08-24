#include <iostream>
#include <thread>
#include <mutex>

class BankAccount {
private:
    double balance = 1000.0;
    std::mutex mtx;
    
public:
    double getBalance() {
        std::lock_guard<std::mutex> lock(mtx);
        return balance;
    }

    void withdraw(double amount) {
        std::lock_guard<std::mutex> lock(mtx);
        if(balance >= amount) {
            balance -= amount;
        }
    }
};

void performTransactions(BankAccount& account) {
    double balance = account.getBalance();
    if(balance > 500.0) {
        account.withdraw(500.0);
        std::cout << "Successfully withdrew 500.0" << std::endl;
    }
}

int main() {
    BankAccount account;
    
    // Spawn two threads to perform transactions.
    std::thread t1(performTransactions, std::ref(account));
    std::thread t2(performTransactions, std::ref(account));
    
    t1.join();
    t2.join();

    std::cout << "Remaining balance: " << account.getBalance() << std::endl;
    return 0;
}