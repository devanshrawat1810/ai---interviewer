#include "db/JsonDailyChallengeRepository.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of JsonDailyChallengeRepository.

JsonDailyChallengeRepository::JsonDailyChallengeRepository(std::string filePath)
    : filePath_(std::move(filePath)) {
    ensureFileExists();
}

void JsonDailyChallengeRepository::ensureFileExists() const {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        fs::path p(filePath_);
        if (p.has_parent_path()) fs::create_directories(p.parent_path());
        if (!fs::exists(p)) {
            std::ofstream out(filePath_);
            out << "[]";
        }
    } catch (...) {}
}

void JsonDailyChallengeRepository::save(const DailyChallengeRecord& record) {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json arr = nlohmann::json::array();
    {
        std::ifstream in(filePath_);
        if (in) {
            try { in >> arr; } catch (...) { arr = nlohmann::json::array(); }
        }
    }
    if (!arr.is_array()) arr = nlohmann::json::array();

    bool updated = false;
    for (auto& item : arr) {
        if (item.value("date", "") == record.date && item.value("course", "") == record.course) {
            item["questionId"] = record.questionId;
            updated = true;
            break;
        }
    }

    if (!updated) {
        nlohmann::json j;
        j["id"] = record.id;
        j["date"] = record.date;
        j["course"] = record.course;
        j["questionId"] = record.questionId;
        arr.push_back(j);
    }

    std::ofstream out(filePath_);
    out << arr.dump(2);
}

std::shared_ptr<DailyChallengeRecord> JsonDailyChallengeRepository::findByDateAndCourse(
    const std::string& date, const std::string& course) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ifstream in(filePath_);
    if (!in) return nullptr;

    nlohmann::json arr;
    try { in >> arr; } catch (...) { return nullptr; }
    if (!arr.is_array()) return nullptr;

    for (const auto& item : arr) {
        if (item.value("date", "") == date && item.value("course", "") == course) {
            auto rec = std::make_shared<DailyChallengeRecord>();
            rec->id = item.value("id", "");
            rec->date = item.value("date", "");
            rec->course = item.value("course", "");
            rec->questionId = item.value("questionId", "");
            return rec;
        }
    }
    return nullptr;
}

std::vector<std::string> JsonDailyChallengeRepository::getPast30DaysQuestionIds(
    const std::string& /* currentDate */, const std::string& course) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> questionIds;
    std::ifstream in(filePath_);
    if (!in) return questionIds;

    nlohmann::json arr;
    try { in >> arr; } catch (...) { return questionIds; }
    if (!arr.is_array()) return questionIds;

    // Collect recent up to 30 challenge question IDs for this course
    for (const auto& item : arr) {
        if (item.value("course", "") == course) {
            questionIds.push_back(item.value("questionId", ""));
        }
    }
    if (questionIds.size() > 30) {
        questionIds.erase(questionIds.begin(), questionIds.end() - 30);
    }
    return questionIds;
}

std::vector<DailyChallengeRecord> JsonDailyChallengeRepository::getAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<DailyChallengeRecord> results;
    std::ifstream in(filePath_);
    if (!in) return results;

    nlohmann::json arr;
    try { in >> arr; } catch (...) { return results; }
    if (!arr.is_array()) return results;

    for (const auto& item : arr) {
        DailyChallengeRecord r;
        r.id = item.value("id", "");
        r.date = item.value("date", "");
        r.course = item.value("course", "");
        r.questionId = item.value("questionId", "");
        results.push_back(r);
    }
    return results;
}
