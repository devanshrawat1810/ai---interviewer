#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/User.h"
#include "core/Student.h"

// OOP Concept: Dependency Inversion Principle (DIP) & Interface Segregation
// Abstract contract for user data persistence. High-level services (AuthService, ConsoleUI)
// depend upon this interface, permitting transparent swapping between JSON and MongoDB backends.
class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual void save(const User& user) = 0;
    virtual std::shared_ptr<User> findById(const std::string& id) = 0;
    virtual std::shared_ptr<User> findByEmail(const std::string& email) = 0;
    virtual std::vector<std::shared_ptr<Student>> getAllStudents() = 0;
    virtual bool deleteById(const std::string& id) = 0;
};
