#include <iostream>
#include <string>
#include <algorithm>

int main() {
    std::string s;
    if (std::cin >> s) {
        // TODO: Reverse the string 's' and print it to std::cout
        std::reverse(s.begin(), s.end());
        std::cout << s << "\n";
    }
    return 0;
}
