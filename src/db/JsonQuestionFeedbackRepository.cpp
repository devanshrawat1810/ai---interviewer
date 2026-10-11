#include "db/JsonQuestionFeedbackRepository.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of JsonQuestionFeedbackRepository.

JsonQuestionFeedbackRepository::JsonQuestionFeedbackRepository(
    std::string reportsFilePath, std::string feedbackFilePath)
    : reportsFilePath_(std::move(reportsFilePath)),
      feedbackFilePath_(std::move(feedbackFilePath)) {
    ensureFilesExist();
}

void JsonQuestionFeedbackRepository::ensureFilesExist() const {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        fs::path p1(reportsFilePath_);
        if (p1.has_parent_path()) fs::create_directories(p1.parent_path());
        if (!fs::exists(p1)) {
            std::ofstream out(reportsFilePath_);
            out << "[]";
        }

        fs::path p2(feedbackFilePath_);
        if (p2.has_parent_path()) fs::create_directories(p2.parent_path());
        if (!fs::exists(p2)) {
            std::ofstream out(feedbackFilePath_);
            out << "[]";
        }
    } catch (...) {}
}

void JsonQuestionFeedbackRepository::saveReport(const QuestionReport& report) {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json arr = nlohmann::json::array();
    {
        std::ifstream in(reportsFilePath_);
        if (in) {
            try { in >> arr; } catch (...) { arr = nlohmann::json::array(); }
        }
    }
    if (!arr.is_array()) arr = nlohmann::json::array();

    nlohmann::json j;
    j["id"] = report.id;
    j["questionId"] = report.questionId;
    j["userId"] = report.userId;
    j["reason"] = report.reason;
    j["reportedAt"] = report.reportedAt;
    arr.push_back(j);

    std::ofstream out(reportsFilePath_);
    out << arr.dump(2);
}

std::vector<QuestionReport> JsonQuestionFeedbackRepository::getAllReports() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<QuestionReport> results;
    std::ifstream in(reportsFilePath_);
    if (!in) return results;

    nlohmann::json arr;
    try { in >> arr; } catch (...) { return results; }
    if (!arr.is_array()) return results;

    for (const auto& j : arr) {
        QuestionReport r;
        r.id = j.value("id", "");
        r.questionId = j.value("questionId", "");
        r.userId = j.value("userId", "");
        r.reason = j.value("reason", "");
        r.reportedAt = j.value("reportedAt", "");
        results.push_back(r);
    }
    return results;
}

void JsonQuestionFeedbackRepository::saveDifficultyFeedback(const DifficultyFeedback& fb) {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json arr = nlohmann::json::array();
    {
        std::ifstream in(feedbackFilePath_);
        if (in) {
            try { in >> arr; } catch (...) { arr = nlohmann::json::array(); }
        }
    }
    if (!arr.is_array()) arr = nlohmann::json::array();

    nlohmann::json j;
    j["id"] = fb.id;
    j["questionId"] = fb.questionId;
    j["userId"] = fb.userId;
    j["feedback"] = fb.feedback;
    j["submittedAt"] = fb.submittedAt;
    arr.push_back(j);

    std::ofstream out(feedbackFilePath_);
    out << arr.dump(2);
}

std::vector<DifficultyFeedback> JsonQuestionFeedbackRepository::getDifficultyFeedbackForQuestion(const std::string& questionId) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<DifficultyFeedback> results;
    std::ifstream in(feedbackFilePath_);
    if (!in) return results;

    nlohmann::json arr;
    try { in >> arr; } catch (...) { return results; }
    if (!arr.is_array()) return results;

    for (const auto& j : arr) {
        if (j.value("questionId", "") == questionId) {
            DifficultyFeedback fb;
            fb.id = j.value("id", "");
            fb.questionId = j.value("questionId", "");
            fb.userId = j.value("userId", "");
            fb.feedback = j.value("feedback", "");
            fb.submittedAt = j.value("submittedAt", "");
            results.push_back(fb);
        }
    }
    return results;
}

std::vector<DifficultyFeedback> JsonQuestionFeedbackRepository::getAllDifficultyFeedback() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<DifficultyFeedback> results;
    std::ifstream in(feedbackFilePath_);
    if (!in) return results;

    nlohmann::json arr;
    try { in >> arr; } catch (...) { return results; }
    if (!arr.is_array()) return results;

    for (const auto& j : arr) {
        DifficultyFeedback fb;
        fb.id = j.value("id", "");
        fb.questionId = j.value("questionId", "");
        fb.userId = j.value("userId", "");
        fb.feedback = j.value("feedback", "");
        fb.submittedAt = j.value("submittedAt", "");
        results.push_back(fb);
    }
    return results;
}
