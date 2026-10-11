#include "db/JsonAiUsageRepository.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

JsonAiUsageRepository::JsonAiUsageRepository(std::string filePath)
    : filePath_(std::move(filePath)) {
    load();
}

void JsonAiUsageRepository::load() {
    std::lock_guard<std::mutex> lock(mutex_);
    usageMap_.clear();
    reviewUsageMap_.clear();

    std::ifstream file(filePath_);
    if (!file.is_open()) return;

    try {
        json j;
        file >> j;
        if (j.contains("usage") && j["usage"].is_object()) {
            for (auto& [uId, dates] : j["usage"].items()) {
                if (dates.is_object()) {
                    for (auto& [date, count] : dates.items()) {
                        usageMap_[uId][date] = count.get<int>();
                    }
                }
            }
        }
        if (j.contains("reviews") && j["reviews"].is_object()) {
            for (auto& [uId, dates] : j["reviews"].items()) {
                if (dates.is_object()) {
                    for (auto& [date, count] : dates.items()) {
                        reviewUsageMap_[uId][date] = count.get<int>();
                    }
                }
            }
        }
    } catch (...) {}
}

void JsonAiUsageRepository::save() {
    json j;
    j["usage"] = usageMap_;
    j["reviews"] = reviewUsageMap_;

    std::ofstream file(filePath_);
    if (file.is_open()) {
        file << j.dump(2);
    }
}

int JsonAiUsageRepository::getUsage(const std::string& userId, const std::string& date) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto itU = usageMap_.find(userId);
    if (itU == usageMap_.end()) return 0;
    auto itD = itU->second.find(date);
    if (itD == itU->second.end()) return 0;
    return itD->second;
}

void JsonAiUsageRepository::incrementUsage(const std::string& userId, const std::string& date, int count) {
    std::lock_guard<std::mutex> lock(mutex_);
    usageMap_[userId][date] += count;
    save();
}

int JsonAiUsageRepository::getReviewUsage(const std::string& userId, const std::string& date) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto itU = reviewUsageMap_.find(userId);
    if (itU == reviewUsageMap_.end()) return 0;
    auto itD = itU->second.find(date);
    if (itD == itU->second.end()) return 0;
    return itD->second;
}

void JsonAiUsageRepository::incrementReviewUsage(const std::string& userId, const std::string& date) {
    std::lock_guard<std::mutex> lock(mutex_);
    reviewUsageMap_[userId][date]++;
    save();
}
