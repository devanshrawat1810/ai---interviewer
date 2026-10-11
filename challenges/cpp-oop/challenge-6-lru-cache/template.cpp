#include <iostream>
#include <unordered_map>
#include <list>
#include <string>

class LRUCache {
private:
    int capacity_;
    std::list<std::pair<int, int>> items_;
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> lookup_;

public:
    explicit LRUCache(int capacity) : capacity_(capacity) {}

    int get(int key) {
        auto it = lookup_.find(key);
        if (it == lookup_.end()) return -1;
        items_.splice(items_.begin(), items_, it->second);
        return it->second->second;
    }

    void put(int key, int value) {
        auto it = lookup_.find(key);
        if (it != lookup_.end()) {
            items_.splice(items_.begin(), items_, it->second);
            it->second->second = value;
            return;
        }

        if (static_cast<int>(items_.size()) >= capacity_) {
            int oldKey = items_.back().first;
            items_.pop_back();
            lookup_.erase(oldKey);
        }

        items_.push_front({key, value});
        lookup_[key] = items_.begin();
    }
};

int main() {
    int cap, n;
    if (!(std::cin >> cap >> n)) return 0;

    LRUCache cache(cap);
    for (int i = 0; i < n; ++i) {
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "get") {
            int k;
            std::cin >> k;
            std::cout << cache.get(k) << "\n";
        } else if (cmd == "put") {
            int k, v;
            std::cin >> k >> v;
            cache.put(k, v);
        }
    }
    return 0;
}
