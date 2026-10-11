#include "auth/AuthService.h"
#include "auth/Sha256.h"
#include "db/IUserRepository.h"
#include "db/DatabaseExceptions.h"
#include <chrono>
#include <iomanip>
#include <sstream>

// OOP Concept: Encapsulation & Single Responsibility Principle
// Implementation of authentication routines, salted hashing, and failed-attempt tracking.

namespace {
std::string getCurrentDateString() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}
} // namespace

AuthService::AuthService(IUserRepository& userRepo, int maxAttempts)
    : userRepo_(userRepo),
      maxAttempts_(maxAttempts) {}

std::shared_ptr<Student> AuthService::registerStudent(const std::string& name,
                                                      const std::string& email,
                                                      const std::string& rawPassword,
                                                      const std::string& course,
                                                      const std::string& college,
                                                      const std::string& branch) {
    auto existing = userRepo_.findByEmail(email);
    if (existing) {
        throw DuplicateKeyException("A user with email '" + email + "' is already registered.");
    }

    std::string salt = crypto::generateSalt(16);
    std::string passwordHash = crypto::sha256(rawPassword + salt);

    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    std::string id = "user_" + std::to_string(millis);

    auto student = std::make_shared<Student>(
        id,
        name,
        email,
        passwordHash,
        salt,
        course,
        college,
        branch,
        0, // streak
        0, // longest streak
        getCurrentDateString()
    );

    userRepo_.save(*student);
    return student;
}

std::shared_ptr<User> AuthService::login(const std::string& email, const std::string& rawPassword) {
    if (isAccountLocked(email)) {
        throw std::runtime_error("Account temporarily locked due to too many failed login attempts (" +
                                 std::to_string(failedAttempts_[email]) + "). Contact admin.");
    }

    auto user = userRepo_.findByEmail(email);
    if (!user) {
        failedAttempts_[email]++;
        throw std::runtime_error("Invalid email or password.");
    }

    std::string expectedHash = crypto::sha256(rawPassword + user->getSalt());
    if (expectedHash != user->getPasswordHash()) {
        failedAttempts_[email]++;
        throw std::runtime_error("Invalid email or password. Attempt " +
                                 std::to_string(failedAttempts_[email]) + " of " +
                                 std::to_string(maxAttempts_) + ".");
    }

    failedAttempts_[email] = 0;

    std::string today = getCurrentDateString();
    if (user->getLastActiveDate() != today) {
        user->setLastActiveDate(today);
        user->addActivityDate(today);
        userRepo_.save(*user);
    }

    return user;
}

void AuthService::resetFailedAttempts(const std::string& email) {
    failedAttempts_[email] = 0;
}

int AuthService::getFailedAttempts(const std::string& email) const {
    auto it = failedAttempts_.find(email);
    return (it != failedAttempts_.end()) ? it->second : 0;
}

bool AuthService::isAccountLocked(const std::string& email) const {
    return getFailedAttempts(email) >= maxAttempts_;
}
