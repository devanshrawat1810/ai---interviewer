#include <iostream>
#include <vector>
#include <string>

// TODO: Implement an encapsulated Stack class
class Stack {
private:
    std::vector<int> data_;

public:
    void push(int val) {
        data_.push_back(val);
    }

    void pop() {
        if (data_.empty()) {
            std::cout << "EMPTY\n";
        } else {
            std::cout << data_.back() << "\n";
            data_.pop_back();
        }
    }

    void peek() const {
        if (data_.empty()) {
            std::cout << "EMPTY\n";
        } else {
            std::cout << data_.back() << "\n";
        }
    }
};

int main() {
    int n;
    if (!(std::cin >> n)) return 0;

    Stack s;
    for (int i = 0; i < n; ++i) {
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "push") {
            int x;
            std::cin >> x;
            s.push(x);
        } else if (cmd == "pop") {
            s.pop();
        } else if (cmd == "peek") {
            s.peek();
        }
    }
    return 0;
}
