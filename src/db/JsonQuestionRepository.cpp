#include "db/JsonQuestionRepository.h"
#include "db/QuestionMapper.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of directory-based JSON question repository.

JsonQuestionRepository::JsonQuestionRepository(std::string directoryPath)
    : directoryPath_(std::move(directoryPath)) {
    try {
        fs::create_directories(directoryPath_);
    } catch (...) {}
}

void JsonQuestionRepository::loadAllQuestions() const {
    if (isLoaded_) return;

    cache_.clear();
    try {
        if (!fs::exists(directoryPath_)) {
            isLoaded_ = true;
            return;
        }

        for (const auto& entry : fs::directory_iterator(directoryPath_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::ifstream in(entry.path());
                if (!in) continue;

                nlohmann::json root;
                try {
                    in >> root;
                } catch (...) {
                    continue;
                }

                if (root.is_array()) {
                    for (const auto& item : root) {
                        auto q = QuestionMapper::fromJson(item);
                        if (q) cache_.push_back(q);
                    }
                } else if (root.is_object() && root.contains("questions") && root["questions"].is_array()) {
                    for (const auto& item : root["questions"]) {
                        auto q = QuestionMapper::fromJson(item);
                        if (q) cache_.push_back(q);
                    }
                } else if (root.is_object()) {
                    auto q = QuestionMapper::fromJson(root);
                    if (q) cache_.push_back(q);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[JsonQuestionRepository] Error loading questions: " << e.what() << "\n";
    }

    isLoaded_ = true;
}

void JsonQuestionRepository::reload() {
    std::lock_guard<std::mutex> lock(mutex_);
    isLoaded_ = false;
    loadAllQuestions();
}

void JsonQuestionRepository::save(const Question& question) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string subject = question.getSubject();
    if (subject.empty()) subject = "general";

    std::string filePath = directoryPath_ + "/" + subject + ".json";

    nlohmann::json qArr = nlohmann::json::array();
    if (fs::exists(filePath)) {
        std::ifstream in(filePath);
        if (in) {
            try {
                in >> qArr;
            } catch (...) {
                qArr = nlohmann::json::array();
            }
        }
    }

    if (!qArr.is_array()) {
        qArr = nlohmann::json::array();
    }

    // Check if updating existing question
    bool updated = false;
    nlohmann::json newJson = QuestionMapper::toJson(question);
    for (auto& item : qArr) {
        if (item.value("id", "") == question.getId()) {
            item = newJson;
            updated = true;
            break;
        }
    }

    if (!updated) {
        qArr.push_back(newJson);
    }

    std::ofstream out(filePath);
    out << qArr.dump(2);

    isLoaded_ = false; // Invalidate cache
}

std::shared_ptr<Question> JsonQuestionRepository::findById(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    loadAllQuestions();

    for (const auto& q : cache_) {
        if (q->getId() == id) {
            return q;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Question>> JsonQuestionRepository::findBySubject(const std::string& subject) {
    std::lock_guard<std::mutex> lock(mutex_);
    loadAllQuestions();

    std::vector<std::shared_ptr<Question>> results;
    for (const auto& q : cache_) {
        if (q->getSubject() == subject) {
            results.push_back(q);
        }
    }
    return results;
}

std::vector<std::shared_ptr<Question>> JsonQuestionRepository::findByDifficulty(Difficulty difficulty) {
    std::lock_guard<std::mutex> lock(mutex_);
    loadAllQuestions();

    std::vector<std::shared_ptr<Question>> results;
    for (const auto& q : cache_) {
        if (q->getDifficulty() == difficulty) {
            results.push_back(q);
        }
    }
    return results;
}

std::vector<std::shared_ptr<Question>> JsonQuestionRepository::findBySubjectAndDifficulty(
    const std::string& subject, Difficulty difficulty) {
    std::lock_guard<std::mutex> lock(mutex_);
    loadAllQuestions();

    std::vector<std::shared_ptr<Question>> results;
    for (const auto& q : cache_) {
        if (q->getSubject() == subject && q->getDifficulty() == difficulty) {
            results.push_back(q);
        }
    }
    return results;
}

std::vector<std::shared_ptr<Question>> JsonQuestionRepository::findByFilters(
    const std::string& /* course */,
    const std::string& subject,
    const std::string& topic,
    Difficulty difficulty) {
    std::lock_guard<std::mutex> lock(mutex_);
    loadAllQuestions();

    std::vector<std::shared_ptr<Question>> results;
    for (const auto& q : cache_) {
        bool matchSubject = subject.empty() || (q->getSubject() == subject);
        bool matchTopic = topic.empty() || (q->getTopic() == topic);
        bool matchDiff = (q->getDifficulty() == difficulty);

        if (matchSubject && matchTopic && matchDiff) {
            results.push_back(q);
        }
    }
    return results;
}

std::vector<std::shared_ptr<Question>> JsonQuestionRepository::getAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    loadAllQuestions();
    return cache_;
}
