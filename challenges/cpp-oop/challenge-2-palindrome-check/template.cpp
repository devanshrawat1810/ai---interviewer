#include <iostream>
#include <string>
#include <algorithm>

int main() {
    std::string s;
    if (std::cin >> s) {
        // TODO: Check if 's' is a palindrome. Output YES or NO.
        std::string rev = s;
        std::reverse(rev.begin(), rev.end());
        if (s == rev) {
            std::cout << "YES\n";
        } else {
            std::cout << "NO\n";
        }
    }
    return 0;
}
