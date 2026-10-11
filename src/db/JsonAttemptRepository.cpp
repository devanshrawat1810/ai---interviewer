#include "db/JsonAttemptRepository.h"
#include "db/AttemptMapper.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of JsonAttemptRepository persisting attempts to flat file.

JsonAttemptRepository::JsonAttemptRepository(std::string filePath)
    : filePath_(std::move(filePath)) {
    ensureFileExists();
}

void JsonAttemptRepository::ensureFileExists() const {
    std::lock_guard<std::mutex> lock(mutex_);
    try {
        fs::path p(filePath_);
        if (p.has_parent_path()) {
            fs::create_directories(p.parent_path());
        }
        if (!fs::exists(p)) {
            std::ofstream out(filePath_);
            out << "[]";
        }
    } catch (...) {}
}

void JsonAttemptRepository::save(const QuestionAttempt& attempt) {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json arr = nlohmann::json::array();

    {
        std::ifstream in(filePath_);
        if (in) {
            try {
                in >> arr;
            } catch (...) {
                arr = nlohmann::json::array();
            }
        }
    }

    if (!arr.is_array()) {
        arr = nlohmann::json::array();
    }

    arr.push_back(AttemptMapper::toJson(attempt));

    std::ofstream out(filePath_);
    out << arr.dump(2);
}

std::vector<QuestionAttempt> JsonAttemptRepository::findByUserId(const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<QuestionAttempt> results;

    std::ifstream in(filePath_);
    if (!in) return results;

    nlohmann::json arr;
    try {
        in >> arr;
    } catch (...) {
        return results;
    }

    if (!arr.is_array()) return results;

    for (const auto& item : arr) {
        if (item.value("userId", "") == userId) {
            results.push_back(AttemptMapper::fromJson(item));
        }
    }
    return results;
}

std::vector<QuestionAttempt> JsonAttemptRepository::findByQuestionId(const std::string& questionId) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<QuestionAttempt> results;

    std::ifstream in(filePath_);
    if (!in) return results;

    nlohmann::json arr;
    try {
        in >> arr;
    } catch (...) {
        return results;
    }

    if (!arr.is_array()) return results;

    for (const auto& item : arr) {
        if (item.value("questionId", "") == questionId) {
            results.push_back(AttemptMapper::fromJson(item));
        }
    }
    return results;
}

std::vector<QuestionAttempt> JsonAttemptRepository::findByUserIdAndDate(
    const std::string& userId, const std::string& date) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<QuestionAttempt> results;

    std::ifstream in(filePath_);
    if (!in) return results;

    nlohmann::json arr;
    try {
        in >> arr;
    } catch (...) {
        return results;
    }

    if (!arr.is_array()) return results;

    for (const auto& item : arr) {
        if (item.value("userId", "") == userId && item.value("date", "") == date) {
            results.push_back(AttemptMapper::fromJson(item));
        }
    }
    return results;
}

std::vector<QuestionAttempt> JsonAttemptRepository::getAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<QuestionAttempt> results;

    std::ifstream in(filePath_);
    if (!in) return results;

    nlohmann::json arr;
    try {
        in >> arr;
    } catch (...) {
        return results;
    }

    if (!arr.is_array()) return results;

    for (const auto& item : arr) {
        results.push_back(AttemptMapper::fromJson(item));
    }
    return results;
}
