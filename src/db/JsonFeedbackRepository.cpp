#include "db/JsonFeedbackRepository.h"
#include "db/FeedbackMapper.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of JsonFeedbackRepository storing evaluation reports in data/feedback.json.

JsonFeedbackRepository::JsonFeedbackRepository(std::string filePath)
    : filePath_(std::move(filePath)) {
    ensureFileExists();
}

void JsonFeedbackRepository::ensureFileExists() const {
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

void JsonFeedbackRepository::save(const FeedbackReport& feedback) {
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

    arr.push_back(FeedbackMapper::toJson(feedback));

    std::ofstream out(filePath_);
    out << arr.dump(2);
}

std::shared_ptr<FeedbackReport> JsonFeedbackRepository::findBySessionId(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ifstream in(filePath_);
    if (!in) return nullptr;

    nlohmann::json arr;
    try {
        in >> arr;
    } catch (...) {
        return nullptr;
    }

    if (!arr.is_array()) return nullptr;

    for (const auto& item : arr) {
        if (item.value("sessionId", "") == sessionId) {
            return std::make_shared<FeedbackReport>(FeedbackMapper::fromJson(item));
        }
    }
    return nullptr;
}

std::vector<FeedbackReport> JsonFeedbackRepository::findByUserId(const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<FeedbackReport> results;

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
            results.push_back(FeedbackMapper::fromJson(item));
        }
    }
    return results;
}

std::vector<FeedbackReport> JsonFeedbackRepository::getAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<FeedbackReport> results;

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
        results.push_back(FeedbackMapper::fromJson(item));
    }
    return results;
}
