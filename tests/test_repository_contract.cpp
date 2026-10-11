#include <gtest/gtest.h>
#include "db/IUserRepository.h"
#include "db/IQuestionRepository.h"
#include "db/IAttemptRepository.h"
#include "db/JsonUserRepository.h"
#include "db/JsonQuestionRepository.h"
#include "db/JsonAttemptRepository.h"
#include "db/MongoUserRepository.h"
#include "db/MongoQuestionRepository.h"
#include "db/MongoAttemptRepository.h"
#include "db/Database.h"
#include "db/DatabaseExceptions.h"
#include "core/Student.h"
#include "core/MCQQuestion.h"
#include <filesystem>

namespace fs = std::filesystem;

// Provider for JSON repositories
struct JsonRepositoryProvider {
    static std::string getUserPath() { return "temp_contract_users.json"; }
    static std::string getAttemptPath() { return "temp_contract_attempts.json"; }
    static std::string getQuestionDir() { return "temp_contract_questions"; }

    static void init() {
        cleanup();
    }

    static void cleanup() {
        try {
            if (fs::exists(getUserPath())) fs::remove(getUserPath());
            if (fs::exists(getAttemptPath())) fs::remove(getAttemptPath());
            if (fs::exists(getQuestionDir())) fs::remove_all(getQuestionDir());
        } catch (...) {}
    }

    static bool isAvailable() {
        return true;
    }

    static std::unique_ptr<IUserRepository> createUserRepo() {
        return std::make_unique<JsonUserRepository>(getUserPath());
    }

    static std::unique_ptr<IAttemptRepository> createAttemptRepo() {
        return std::make_unique<JsonAttemptRepository>(getAttemptPath());
    }

    static std::unique_ptr<IQuestionRepository> createQuestionRepo() {
        return std::make_unique<JsonQuestionRepository>(getQuestionDir());
    }
};

// Provider for MongoDB repositories
struct MongoRepositoryProvider {
    static void init() {
        Database::getInstance().connect();
    }

    static void cleanup() {
        // Cleanup contract test user if exists
        try {
            auto uRepo = createUserRepo();
            uRepo->deleteById("contract_test_u1");
            uRepo->deleteById("contract_test_u2");
        } catch (...) {}
    }

    static bool isAvailable() {
#ifdef AI_INTERVIEWER_HAS_MONGODB
        return Database::getInstance().isConnected() && Database::getInstance().ping();
#else
        return false;
#endif
    }

    static std::unique_ptr<IUserRepository> createUserRepo() {
        return std::make_unique<MongoUserRepository>();
    }

    static std::unique_ptr<IAttemptRepository> createAttemptRepo() {
        return std::make_unique<MongoAttemptRepository>();
    }

    static std::unique_ptr<IQuestionRepository> createQuestionRepo() {
        return std::make_unique<MongoQuestionRepository>();
    }
};

template <typename TProvider>
class RepositoryContractTest : public ::testing::Test {
protected:
    void SetUp() override {
        TProvider::init();
        if (!TProvider::isAvailable()) {
            GTEST_SKIP() << "Database provider is not available or unreachable. Skipping integration test.";
        }
    }

    void TearDown() override {
        TProvider::cleanup();
    }
};

using Providers = ::testing::Types<JsonRepositoryProvider, MongoRepositoryProvider>;
TYPED_TEST_SUITE(RepositoryContractTest, Providers);

TYPED_TEST(RepositoryContractTest, SaveAndFindUser) {
    auto repo = TypeParam::createUserRepo();
    Student student("contract_test_u1", "Grace Hopper", "grace@contract.edu", "hash", "salt", "CSE");
    repo->save(student);

    auto found = repo->findById("contract_test_u1");
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->getName(), "Grace Hopper");
    EXPECT_EQ(found->getEmail(), "grace@contract.edu");

    auto foundEmail = repo->findByEmail("grace@contract.edu");
    ASSERT_NE(foundEmail, nullptr);
    EXPECT_EQ(foundEmail->getId(), "contract_test_u1");
}

TYPED_TEST(RepositoryContractTest, UniqueEmailRejection) {
    auto repo = TypeParam::createUserRepo();
    Student student1("contract_test_u1", "User One", "unique@contract.edu", "hash1", "salt1", "CSE");
    repo->save(student1);

    // Save user with DIFFERENT ID but SAME email must throw DuplicateKeyException
    Student student2("contract_test_u2", "User Two", "unique@contract.edu", "hash2", "salt2", "ECE");
    EXPECT_THROW(repo->save(student2), DuplicateKeyException);
}

TYPED_TEST(RepositoryContractTest, UpdateStreak) {
    auto repo = TypeParam::createUserRepo();
    Student student("contract_test_u1", "Streak Tester", "streak@contract.edu", "hash", "salt", "CSE", "Unspecified", "Unspecified", 1);
    repo->save(student);

    student.incrementStreak();
    student.setLastActiveDate("2026-10-09");
    repo->save(student);

    auto updated = repo->findById("contract_test_u1");
    ASSERT_NE(updated, nullptr);
    EXPECT_EQ(updated->getStreak(), 2);
    EXPECT_EQ(updated->getLastActiveDate(), "2026-10-09");
}

TYPED_TEST(RepositoryContractTest, SaveAndQueryAttemptsByUser) {
    auto repo = TypeParam::createAttemptRepo();
    QuestionAttempt att1;
    att1.id = "att_c1";
    att1.userId = "contract_user";
    att1.questionId = "q1";
    att1.difficulty = Difficulty::EASY;
    att1.isCorrect = true;
    att1.pointsEarned = 1;
    att1.date = "2026-10-09";

    QuestionAttempt att2;
    att2.id = "att_c2";
    att2.userId = "contract_user";
    att2.questionId = "q2";
    att2.difficulty = Difficulty::HARD;
    att2.isCorrect = false;
    att2.pointsEarned = 0;
    att2.date = "2026-10-09";

    repo->save(att1);
    repo->save(att2);

    auto userAttempts = repo->findByUserId("contract_user");
    EXPECT_GE(userAttempts.size(), 2u);
}

TYPED_TEST(RepositoryContractTest, QueryQuestionsByDifficulty) {
    auto repo = TypeParam::createQuestionRepo();
    auto q1 = std::make_shared<MCQQuestion>(
        "q_contract_1", "cpp", "oop", Difficulty::HARD, "Hard Q1", "Exp", std::vector<std::string>{"A"}, "A");
    auto q2 = std::make_shared<MCQQuestion>(
        "q_contract_2", "cpp", "oop", Difficulty::EASY, "Easy Q1", "Exp", std::vector<std::string>{"A"}, "A");

    repo->save(*q1);
    repo->save(*q2);

    auto hardQuestions = repo->findByDifficulty(Difficulty::HARD);
    EXPECT_GE(hardQuestions.size(), 1u);
    for (const auto& q : hardQuestions) {
        EXPECT_EQ(q->getDifficulty(), Difficulty::HARD);
    }
}
