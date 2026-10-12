#pragma once

#include "core/User.h"

// OOP Concept: Inheritance
// Admin specializes User with elevated system maintenance permissions.
class Admin : public User {
public:
    Admin(std::string id,
          std::string name,
          std::string email,
          std::string passwordHash,
          std::string salt,
          std::string course = "ADMIN",
          std::string college = "Administration",
          std::string branch = "Staff",
          int streak = 0,
          int longestStreak = 0,
          std::string lastActiveDate = "");

    ~Admin() override = default;

    bool isAdmin() const noexcept override;
};
