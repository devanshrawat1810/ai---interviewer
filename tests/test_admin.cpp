#include <gtest/gtest.h>
#include "admin/AdminService.h"
#include "core/Admin.h"
#include "core/Student.h"
#include "core/MCQQuestion.h"
#include "core/TextQuestion.h"
#include "db/JsonUserRepository.h"
#include "db/JsonQuestionRepository.h"
#include "db/JsonAttemptRepository.h"
#include "db/JsonQuestionFeedbackRepository.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

class AdminServiceTest : public ::testing::Test {
protected:
    std::string testUsersPath = "temp_admin_users.json";
    std::string testQuestionsDir = "temp_admin_questions";
    std::string testAttemptsPath = "temp_admin_attempts.json";
    std::string testReportsPath = "temp_admin_reports.json";
    std::string testFbPath = "temp_admin_fb.json";
    std::string testBulkJson = "temp_bulk_questions.json";

    std::unique_ptr<JsonUserRepository> userRepo;
    std::unique_ptr<JsonQuestionRepository> questionRepo;
    std::unique_ptr<JsonAttemptRepository> attemptRepo;
    std::unique_ptr<JsonQuestionFeedbackRepository> feedbackRepo;
    std::unique_ptr<AdminService> adminService;

    Admin admin{"admin_1", "Admin User", "admin@college.edu", "hash", "salt", "ADMIN"};
    Student student{"student_1", "Regular Student", "student@college.edu", "hash", "salt", "CSE"};

    void SetUp() override {
        cleanup();
        userRepo = std::make_unique<JsonUserRepository>(testUsersPath);
        questionRepo = std::make_unique<JsonQuestionRepository>(testQuestionsDir);
        attemptRepo = std::make_unique<JsonAttemptRepository>(testAttemptsPath);
        feedbackRepo = std::make_unique<JsonQuestionFeedbackRepository>(testReportsPath, testFbPath);

        adminService = std::make_unique<AdminService>(
            *userRepo, *questionRepo, *attemptRepo, *feedbackRepo);
    }

    void TearDown() override {
        cleanup();
    }

    void cleanup() {
        try {
            if (fs::exists(testUsersPath)) fs::remove(testUsersPath);
            if (fs::exists(testQuestionsDir)) fs::remove_all(testQuestionsDir);
            if (fs::exists(testAttemptsPath)) fs::remove(testAttemptsPath);
            if (fs::exists(testReportsPath)) fs::remove(testReportsPath);
            if (fs::exists(testFbPath)) fs::remove(testFbPath);
            if (fs::exists(testBulkJson)) fs::remove(testBulkJson);
        } catch (...) {}
    }
};

TEST_F(AdminServiceTest, NonAdminCallsAreRejected) {
    MCQQuestion q("q_test", "cpp", "oop", Difficulty::EASY, "Valid text", "Exp", {"A", "B"}, "A");

    // Student attempting to add a question must be rejected by service-level RBAC check
    EXPECT_THROW(adminService->addQuestion(student, q), std::runtime_error);
    EXPECT_THROW(adminService->softDeleteQuestion(student, "q_test"), std::runtime_error);
    EXPECT_THROW(adminService->searchQuestions(student, "query"), std::runtime_error);
    EXPECT_THROW(adminService->listReportedQuestions(student), std::runtime_error);
    EXPECT_THROW(adminService->getUserStats(student, "2026-10-11"), std::runtime_error);
}

TEST_F(AdminServiceTest, QuestionValidationRules) {
    // Empty text
    MCQQuestion invalidText("", "cpp", "oop", Difficulty::EASY, "", "Exp", {"A", "B"}, "A");
    EXPECT_THROW(adminService->addQuestion(admin, invalidText), ValidationException);

    // MCQ with less than 2 options
    MCQQuestion mcqOneOpt("mcq_bad", "cpp", "oop", Difficulty::EASY, "Valid Text", "Exp", {"OnlyOne"}, "OnlyOne");
    EXPECT_THROW(adminService->addQuestion(admin, mcqOneOpt), ValidationException);

    // MCQ with empty answer
    MCQQuestion mcqEmptyAns("mcq_bad2", "cpp", "oop", Difficulty::EASY, "Valid Text", "Exp", {"A", "B"}, "");
    EXPECT_THROW(adminService->addQuestion(admin, mcqEmptyAns), ValidationException);

    // TextQuestion with empty keywords
    TextQuestion txtNoKeys("txt_bad", "cpp", "oop", Difficulty::EASY, "Valid Text", "Exp", "sample", {});
    EXPECT_THROW(adminService->addQuestion(admin, txtNoKeys), ValidationException);

    // Valid question passes validation
    MCQQuestion validQ("q_valid", "cpp", "oop", Difficulty::EASY, "Valid Text", "Exp", {"A", "B"}, "A");
    EXPECT_NO_THROW(adminService->addQuestion(admin, validQ));
}

TEST_F(AdminServiceTest, SoftDeletePreservesHistoricalAttempts) {
    auto q = std::make_shared<MCQQuestion>("q_hist", "cpp", "oop", Difficulty::EASY, "Hist Text", "Exp", {"A", "B"}, "A");
    questionRepo->save(*q);

    // Record an attempt on this question
    QuestionAttempt att;
    att.id = "att_hist";
    att.userId = "student_1";
    att.questionId = "q_hist";
    att.isCorrect = true;
    att.date = "2026-10-11";
    attemptRepo->save(att);

    // Admin soft-deletes question
    adminService->softDeleteQuestion(admin, "q_hist");

    // Question is still fetchable by ID for historical attempt review, but marked inactive
    auto fetched = questionRepo->findById("q_hist");
    ASSERT_NE(fetched, nullptr);
    EXPECT_FALSE(fetched->isActive());

    // Historical attempt still resolves question
    auto userAttempts = attemptRepo->findByUserId("student_1");
    ASSERT_EQ(userAttempts.size(), 1u);
    EXPECT_EQ(userAttempts[0].questionId, "q_hist");
}

TEST_F(AdminServiceTest, BulkImportPerLineReport) {
    // Write sample JSON with 2 valid and 1 invalid question
    {
        std::ofstream out(testBulkJson);
        out << R"([
            {
                "id": "bulk_1",
                "subject": "cpp",
                "topic": "basics",
                "difficulty": "EASY",
                "type": "mcq",
                "text": "Valid Q1?",
                "options": ["A", "B"],
                "answer": "A"
            },
            {
                "id": "bulk_2",
                "subject": "cpp",
                "topic": "basics",
                "difficulty": "EASY",
                "type": "mcq",
                "text": "Invalid Q2?",
                "options": ["SingleOptionOnly"],
                "answer": "SingleOptionOnly"
            },
            {
                "id": "bulk_3",
                "subject": "cpp",
                "topic": "basics",
                "difficulty": "MEDIUM",
                "type": "text",
                "text": "Valid Q3 text?",
                "sampleAnswer": "sample",
                "keywords": ["key1"]
            }
        ])";
    }

    auto report = adminService->bulkImportQuestions(admin, testBulkJson);
    EXPECT_EQ(report.totalRead, 3);
    EXPECT_EQ(report.accepted, 2);
    EXPECT_EQ(report.rejected, 1);
    ASSERT_EQ(report.errorDetails.size(), 1u);
    EXPECT_NE(report.errorDetails[0].find("at least 2 options"), std::string::npos);
}
