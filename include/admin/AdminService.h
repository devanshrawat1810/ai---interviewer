#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "core/User.h"
#include "core/Question.h"
#include "db/IQuestionRepository.h"
#include "db/IUserRepository.h"
#include "db/IAttemptRepository.h"
#include "db/IQuestionFeedbackRepository.h"
#include "admin/ValidationException.h"

struct UserStats {
    int totalUsers{0};
    int studentsCount{0};
    int adminsCount{0};
    int activeTodayCount{0};
    std::map<std::string, int> attemptsPerSubject;
};

struct BulkImportReport {
    int totalRead{0};
    int accepted{0};
    int rejected{0};
    std::vector<std::string> errorDetails;
};

// OOP Concept: Single Responsibility Principle & Role-Based Access Control (RBAC)
// Encapsulates administrative workflows with strict service-level role validation.
class AdminService {
private:
    IUserRepository& userRepo_;
    IQuestionRepository& questionRepo_;
    IAttemptRepository& attemptRepo_;
    IQuestionFeedbackRepository& feedbackRepo_;

    void requireAdminRole(const User& caller) const;

public:
    AdminService(IUserRepository& userRepo,
                 IQuestionRepository& questionRepo,
                 IAttemptRepository& attemptRepo,
                 IQuestionFeedbackRepository& feedbackRepo);
    ~AdminService() = default;

    // Static validator enforcing domain invariants
    static void validateQuestion(const Question& question);

    void addQuestion(const User& caller, const Question& question);
    void editQuestion(const User& caller, const Question& question);
    void softDeleteQuestion(const User& caller, const std::string& questionId);

    std::vector<std::shared_ptr<Question>> searchQuestions(
        const User& caller, const std::string& query) const;

    std::vector<QuestionReport> listReportedQuestions(const User& caller) const;
    UserStats getUserStats(const User& caller, const std::string& todayDate) const;
    BulkImportReport bulkImportQuestions(const User& caller, const std::string& jsonFilePath);

    std::vector<std::pair<std::string, std::string>> getDifficultyDiscrepancies(
        const User& caller) const;
};
