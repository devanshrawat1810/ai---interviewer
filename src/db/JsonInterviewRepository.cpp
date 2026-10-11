#include "db/JsonInterviewRepository.h"
#include "db/InterviewMapper.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of JsonInterviewRepository persisting session records to data/interviews.json.

JsonInterviewRepository::JsonInterviewRepository(std::string filePath)
    : filePath_(std::move(filePath)) {
    ensureFileExists();
}

void JsonInterviewRepository::ensureFileExists() const {
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

void JsonInterviewRepository::save(const StoredInterviewSession& session) {
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

    arr.push_back(InterviewMapper::toJson(session));

    std::ofstream out(filePath_);
    out << arr.dump(2);
}

std::shared_ptr<StoredInterviewSession> JsonInterviewRepository::findById(const std::string& id) {
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
        if (item.value("id", "") == id) {
            return std::make_shared<StoredInterviewSession>(InterviewMapper::fromJson(item));
        }
    }
    return nullptr;
}

std::vector<StoredInterviewSession> JsonInterviewRepository::findByUserId(const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<StoredInterviewSession> results;

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
            results.push_back(InterviewMapper::fromJson(item));
        }
    }
    return results;
}

std::vector<StoredInterviewSession> JsonInterviewRepository::getAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<StoredInterviewSession> results;

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
        results.push_back(InterviewMapper::fromJson(item));
    }
    return results;
}
