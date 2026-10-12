#include "core/Admin.h"

// OOP Concept: Inheritance
// Implements Admin privileges and constructors.

Admin::Admin(std::string id,
             std::string name,
             std::string email,
             std::string passwordHash,
             std::string salt,
             std::string course,
             std::string college,
             std::string branch,
             int streak,
             int longestStreak,
             std::string lastActiveDate)
    : User(std::move(id),
           std::move(name),
           std::move(email),
           std::move(passwordHash),
           std::move(salt),
           "admin",
           std::move(course),
           std::move(college),
           std::move(branch),
           streak,
           longestStreak,
           std::move(lastActiveDate)) {}

bool Admin::isAdmin() const noexcept {
    return true;
}
