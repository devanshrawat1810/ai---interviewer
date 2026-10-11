#include "admin/AdminService.h"
#include "core/MCQQuestion.h"
#include "core/TextQuestion.h"
#include "db/QuestionMapper.h"
#include <fstream>
#include <algorithm>
#include <nlohmann/json.hpp>

// OOP Concept: Single Responsibility Principle & Role-Based Access Control
// Implementation of AdminService.

AdminService::AdminService(IUserRepository& userRepo,
                           IQuestionRepository& questionRepo,
                           IAttemptRepository& attemptRepo,
                           IQuestionFeedbackRepository& feedbackRepo)
    : userRepo_(userRepo),
      questionRepo_(questionRepo),
      attemptRepo_(attemptRepo),
      feedbackRepo_(feedbackRepo) {}

void AdminService::requireAdminRole(const User& caller) const {
    if (!caller.isAdmin()) {
        throw std::runtime_error("Unauthorized: Action requires Admin role.");
    }
}

void AdminService::validateQuestion(const Question& question) {
    if (question.getId().empty()) {
        throw ValidationException("Question ID cannot be empty.");
    }
    if (question.getSubject().empty()) {
        throw ValidationException("Question subject cannot be empty.");
    }
    if (question.getTopic().empty()) {
        throw ValidationException("Question topic cannot be empty.");
    }
    if (question.getText().empty()) {
        throw ValidationException("Question text cannot be empty.");
    }

    if (const auto* mcq = dynamic_cast<const MCQQuestion*>(&question)) {
        if (mcq->getOptions().size() < 2) {
            throw ValidationException("MCQ Question must contain at least 2 options.");
        }
        if (mcq->getCorrectAnswer().empty()) {
            throw ValidationException("MCQ Question must define a non-empty correct answer.");
        }
    } else if (const auto* txt = dynamic_cast<const TextQuestion*>(&question)) {
        if (txt->getKeywords().empty()) {
            throw ValidationException("Text Question must contain at least one evaluation keyword.");
        }
    }
}

void AdminService::addQuestion(const User& caller, const Question& question) {
    requireAdminRole(caller);
    validateQuestion(question);
    questionRepo_.save(question);
}

void AdminService::editQuestion(const User& caller, const Question& question) {
    requireAdminRole(caller);
    validateQuestion(question);
    questionRepo_.save(question);
}

void AdminService::softDeleteQuestion(const User& caller, const std::string& questionId) {
    requireAdminRole(caller);
    auto q = questionRepo_.findById(questionId);
    if (!q) {
        throw std::runtime_error("Question with ID '" + questionId + "' not found.");
    }
    q->setActive(false);
    questionRepo_.save(*q);
}

std::vector<std::shared_ptr<Question>> AdminService::searchQuestions(
    const User& caller, const std::string& query) const {
    requireAdminRole(caller);
    auto allQuestions = questionRepo_.getAll();
    if (query.empty()) return allQuestions;

    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    std::vector<std::shared_ptr<Question>> results;
    for (const auto& q : allQuestions) {
        std::string text = q->getText();
        std::string subject = q->getSubject();
        std::string topic = q->getTopic();
        std::string id = q->getId();

        std::transform(text.begin(), text.end(), text.begin(), ::tolower);
        std::transform(subject.begin(), subject.end(), subject.begin(), ::tolower);
        std::transform(topic.begin(), topic.end(), topic.begin(), ::tolower);
        std::transform(id.begin(), id.end(), id.begin(), ::tolower);

        if (text.find(lowerQuery) != std::string::npos ||
            subject.find(lowerQuery) != std::string::npos ||
            topic.find(lowerQuery) != std::string::npos ||
            id.find(lowerQuery) != std::string::npos) {
            results.push_back(q);
        }
    }
    return results;
}

std::vector<QuestionReport> AdminService::listReportedQuestions(const User& caller) const {
    requireAdminRole(caller);
    return feedbackRepo_.getAllReports();
}

UserStats AdminService::getUserStats(const User& caller, const std::string& todayDate) const {
    requireAdminRole(caller);
    UserStats stats;

    auto students = userRepo_.getAllStudents();
    stats.studentsCount = static_cast<int>(students.size());
    stats.totalUsers = stats.studentsCount;

    for (const auto& s : students) {
        if (s->getLastActiveDate() == todayDate) {
            stats.activeTodayCount++;
        }
    }

    auto attempts = attemptRepo_.getAll();
    for (const auto& att : attempts) {
        stats.attemptsPerSubject[att.subject]++;
    }

    return stats;
}

BulkImportReport AdminService::bulkImportQuestions(const User& caller, const std::string& jsonFilePath) {
    requireAdminRole(caller);
    BulkImportReport report;

    std::ifstream in(jsonFilePath);
    if (!in) {
        report.errorDetails.push_back("Could not open JSON file: " + jsonFilePath);
        return report;
    }

    nlohmann::json root;
    try {
        in >> root;
    } catch (const std::exception& e) {
        report.errorDetails.push_back("JSON parsing error: " + std::string(e.what()));
        return report;
    }

    auto processItem = [&](const nlohmann::json& item, int lineNum) {
        report.totalRead++;
        try {
            auto q = QuestionMapper::fromJson(item);
            if (!q) {
                report.rejected++;
                report.errorDetails.push_back("Item #" + std::to_string(lineNum) + ": Failed to instantiate question object.");
                return;
            }
            validateQuestion(*q);
            questionRepo_.save(*q);
            report.accepted++;
        } catch (const ValidationException& ve) {
            report.rejected++;
            report.errorDetails.push_back("Item #" + std::to_string(lineNum) + ": " + ve.what());
        } catch (const std::exception& e) {
            report.rejected++;
            report.errorDetails.push_back("Item #" + std::to_string(lineNum) + ": " + e.what());
        }
    };

    if (root.is_array()) {
        int idx = 1;
        for (const auto& item : root) {
            processItem(item, idx++);
        }
    } else if (root.is_object() && root.contains("questions") && root["questions"].is_array()) {
        int idx = 1;
        for (const auto& item : root["questions"]) {
            processItem(item, idx++);
        }
    } else if (root.is_object()) {
        processItem(root, 1);
    }

    return report;
}

std::vector<std::pair<std::string, std::string>> AdminService::getDifficultyDiscrepancies(
    const User& caller) const {
    requireAdminRole(caller);
    std::vector<std::pair<std::string, std::string>> discrepancies;

    auto allFeedback = feedbackRepo_.getAllDifficultyFeedback();
    std::map<std::string, std::pair<int, int>> counts; // qId -> {tooEasy, tooHard}

    for (const auto& fb : allFeedback) {
        if (fb.feedback == "too_easy") counts[fb.questionId].first++;
        else if (fb.feedback == "too_hard") counts[fb.questionId].second++;
    }

    for (const auto& entry : counts) {
        auto q = questionRepo_.findById(entry.first);
        if (!q) continue;

        int tooEasy = entry.second.first;
        int tooHard = entry.second.second;
        Difficulty diff = q->getDifficulty();

        if (diff == Difficulty::HARD && tooEasy > tooHard && tooEasy >= 2) {
            discrepancies.push_back({q->getId(), "Marked HARD, but " + std::to_string(tooEasy) +
                                                 " students reported 'too easy'."});
        } else if (diff == Difficulty::EASY && tooHard > tooEasy && tooHard >= 2) {
            discrepancies.push_back({q->getId(), "Marked EASY, but " + std::to_string(tooHard) +
                                                 " students reported 'too hard'."});
        }
    }

    return discrepancies;
}
