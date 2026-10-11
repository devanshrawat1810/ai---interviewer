#pragma once

#include <string>
#include <memory>
#include <map>
#include "core/User.h"
#include "core/Student.h"

class IUserRepository;

// OOP Concept: Encapsulation & Single Responsibility Principle
// AuthService manages user authentication, credential validation, salt generation,
// and security throttling for brute-force protection.
class AuthService {
private:
    IUserRepository& userRepo_;
    std::map<std::string, int> failedAttempts_;
    const int maxAttempts_{3};

public:
    explicit AuthService(IUserRepository& userRepo, int maxAttempts = 3);
    ~AuthService() = default;

    std::shared_ptr<Student> registerStudent(const std::string& name,
                                             const std::string& email,
                                             const std::string& rawPassword,
                                             const std::string& course = "CSE",
                                             const std::string& college = "Unspecified",
                                             const std::string& branch = "Unspecified");

    std::shared_ptr<User> login(const std::string& email, const std::string& rawPassword);

    void resetFailedAttempts(const std::string& email);
    int getFailedAttempts(const std::string& email) const;
    bool isAccountLocked(const std::string& email) const;
};
