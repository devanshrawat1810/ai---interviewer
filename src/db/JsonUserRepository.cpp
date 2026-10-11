#include "db/JsonUserRepository.h"
#include "db/UserMapper.h"
#include "db/DatabaseExceptions.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// OOP Concept: Interface Implementation & Polymorphism
// Implements flat file JSON storage for IUserRepository with unique email enforcement.

JsonUserRepository::JsonUserRepository(std::string filePath)
    : filePath_(std::move(filePath)) {
    ensureFileExists();
}

void JsonUserRepository::ensureFileExists() const {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        fs::path p(filePath_);
        if (p.has_parent_path()) {
            fs::create_directories(p.parent_path());
        }
        if (!fs::exists(p)) {
            std::ofstream out(filePath_);
            out << "[]";
        }
    } catch (...) {}
}

void JsonUserRepository::save(const User& user) {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json usersArr = nlohmann::json::array();

    {
        std::ifstream in(filePath_);
        if (in) {
            try {
                in >> usersArr;
            } catch (...) {
                usersArr = nlohmann::json::array();
            }
        }
    }

    if (!usersArr.is_array()) {
        usersArr = nlohmann::json::array();
    }

    // Check for duplicate email with a different user ID
    for (const auto& item : usersArr) {
        std::string existingEmail = item.value("email", "");
        std::string existingId = item.value("id", "");
        if (existingEmail == user.getEmail() && existingId != user.getId()) {
            throw DuplicateKeyException("User with email '" + user.getEmail() + "' already exists.");
        }
    }

    // Update if already exists with same ID, otherwise insert
    bool updated = false;
    nlohmann::json userJson = UserMapper::toJson(user);

    for (auto& item : usersArr) {
        if (item.value("id", "") == user.getId()) {
            item = userJson;
            updated = true;
            break;
        }
    }

    if (!updated) {
        usersArr.push_back(userJson);
    }

    std::ofstream out(filePath_);
    out << usersArr.dump(2);
}

std::shared_ptr<User> JsonUserRepository::findById(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ifstream in(filePath_);
    if (!in) return nullptr;

    nlohmann::json usersArr;
    try {
        in >> usersArr;
    } catch (...) {
        return nullptr;
    }

    if (!usersArr.is_array()) return nullptr;

    for (const auto& item : usersArr) {
        if (item.value("id", "") == id) {
            return UserMapper::fromJson(item);
        }
    }
    return nullptr;
}

std::shared_ptr<User> JsonUserRepository::findByEmail(const std::string& email) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ifstream in(filePath_);
    if (!in) return nullptr;

    nlohmann::json usersArr;
    try {
        in >> usersArr;
    } catch (...) {
        return nullptr;
    }

    if (!usersArr.is_array()) return nullptr;

    for (const auto& item : usersArr) {
        if (item.value("email", "") == email) {
            return UserMapper::fromJson(item);
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Student>> JsonUserRepository::getAllStudents() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::shared_ptr<Student>> students;

    std::ifstream in(filePath_);
    if (!in) return students;

    nlohmann::json usersArr;
    try {
        in >> usersArr;
    } catch (...) {
        return students;
    }

    if (!usersArr.is_array()) return students;

    for (const auto& item : usersArr) {
        auto user = UserMapper::fromJson(item);
        if (user && !user->isAdmin()) {
            auto student = std::dynamic_pointer_cast<Student>(user);
            if (student) {
                students.push_back(student);
            }
        }
    }
    return students;
}

bool JsonUserRepository::deleteById(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ifstream in(filePath_);
    if (!in) return false;

    nlohmann::json usersArr;
    try {
        in >> usersArr;
    } catch (...) {
        return false;
    }

    if (!usersArr.is_array()) return false;

    nlohmann::json filtered = nlohmann::json::array();
    bool found = false;
    for (const auto& item : usersArr) {
        if (item.value("id", "") == id) {
            found = true;
        } else {
            filtered.push_back(item);
        }
    }

    if (found) {
        std::ofstream out(filePath_);
        out << filtered.dump(2);
    }
    return found;
}
