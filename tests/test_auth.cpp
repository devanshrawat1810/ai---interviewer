#include <gtest/gtest.h>
#include "auth/Sha256.h"
#include "auth/AuthService.h"
#include "db/JsonUserRepository.h"
#include "db/DatabaseExceptions.h"
#include <filesystem>

namespace fs = std::filesystem;

class AuthTest : public ::testing::Test {
protected:
    std::string testDbPath = "temp_auth_test_users.json";
    std::unique_ptr<JsonUserRepository> userRepo;
    std::unique_ptr<AuthService> authService;

    void SetUp() override {
        if (fs::exists(testDbPath)) {
            fs::remove(testDbPath);
        }
        userRepo = std::make_unique<JsonUserRepository>(testDbPath);
        authService = std::make_unique<AuthService>(*userRepo, 3);
    }

    void TearDown() override {
        if (fs::exists(testDbPath)) {
            fs::remove(testDbPath);
        }
    }
};

TEST_F(AuthTest, Sha256KnownVector) {
    // Standard test vector: sha256("")
    EXPECT_EQ(crypto::sha256(""), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    // Standard test vector: sha256("hello")
    EXPECT_EQ(crypto::sha256("hello"), "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824");
}

TEST_F(AuthTest, SaltGenerationUnique) {
    std::string salt1 = crypto::generateSalt(16);
    std::string salt2 = crypto::generateSalt(16);
    EXPECT_EQ(salt1.length(), 16u);
    EXPECT_EQ(salt2.length(), 16u);
    EXPECT_NE(salt1, salt2);
}

TEST_F(AuthTest, RegisterAndLoginSuccess) {
    auto student = authService->registerStudent("John Doe", "john@college.edu", "secret123", "CSE");
    ASSERT_NE(student, nullptr);
    EXPECT_EQ(student->getName(), "John Doe");
    EXPECT_EQ(student->getEmail(), "john@college.edu");
    EXPECT_FALSE(student->getPasswordHash().empty());
    EXPECT_NE(student->getPasswordHash(), "secret123"); // Salted hash, not plaintext

    auto loggedIn = authService->login("john@college.edu", "secret123");
    ASSERT_NE(loggedIn, nullptr);
    EXPECT_EQ(loggedIn->getId(), student->getId());
}

TEST_F(AuthTest, LoginFailureIncrementsAttemptsAndLocks) {
    authService->registerStudent("Jane Doe", "jane@college.edu", "pass123", "ECE");

    // 1st failed attempt
    EXPECT_THROW(authService->login("jane@college.edu", "wrongpass1"), std::runtime_error);
    EXPECT_EQ(authService->getFailedAttempts("jane@college.edu"), 1);

    // 2nd failed attempt
    EXPECT_THROW(authService->login("jane@college.edu", "wrongpass2"), std::runtime_error);
    EXPECT_EQ(authService->getFailedAttempts("jane@college.edu"), 2);

    // 3rd failed attempt
    EXPECT_THROW(authService->login("jane@college.edu", "wrongpass3"), std::runtime_error);
    EXPECT_EQ(authService->getFailedAttempts("jane@college.edu"), 3);
    EXPECT_TRUE(authService->isAccountLocked("jane@college.edu"));

    // 4th attempt: Even with the CORRECT password, account is locked!
    EXPECT_THROW(authService->login("jane@college.edu", "pass123"), std::runtime_error);
}

TEST_F(AuthTest, DuplicateEmailRejection) {
    authService->registerStudent("First", "duplicate@college.edu", "pass1", "CSE");
    EXPECT_THROW(authService->registerStudent("Second", "duplicate@college.edu", "pass2", "CSE"),
                 DuplicateKeyException);
}
