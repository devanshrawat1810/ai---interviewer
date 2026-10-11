#include "db/MongoAiUsageRepository.h"
#include "db/Database.h"
#include "db/DatabaseExceptions.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/options/update.hpp>
#endif

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of MongoAiUsageRepository storing daily AI telemetry in the "ai_usage" collection.

int MongoAiUsageRepository::getUsage(const std::string& userId, const std::string& date) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("ai_usage");
        bsoncxx::builder::stream::document filter;
        filter << "userId" << userId << "date" << date;

        auto result = col.find_one(filter.view());
        if (!result) return 0;

        auto view = result->view();
        if (view["usage"]) {
            return view["usage"].get_int32().value;
        }
        return 0;
    } catch (...) {
        std::lock_guard<std::mutex> lock(mutex_);
        return inMemoryUsageFallback_[userId][date];
    }
#else
    std::lock_guard<std::mutex> lock(mutex_);
    return inMemoryUsageFallback_[userId][date];
#endif
}

void MongoAiUsageRepository::incrementUsage(const std::string& userId, const std::string& date, int count) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("ai_usage");
        bsoncxx::builder::stream::document filter;
        filter << "userId" << userId << "date" << date;

        bsoncxx::builder::stream::document updateDoc;
        updateDoc << "$inc" << bsoncxx::builder::stream::open_document
                  << "usage" << count
                  << bsoncxx::builder::stream::close_document;

        mongocxx::options::update opts;
        opts.upsert(true);
        col.update_one(filter.view(), updateDoc.view(), opts);
    } catch (...) {
        std::lock_guard<std::mutex> lock(mutex_);
        inMemoryUsageFallback_[userId][date] += count;
    }
#else
    std::lock_guard<std::mutex> lock(mutex_);
    inMemoryUsageFallback_[userId][date] += count;
#endif
}

int MongoAiUsageRepository::getReviewUsage(const std::string& userId, const std::string& date) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("ai_usage");
        bsoncxx::builder::stream::document filter;
        filter << "userId" << userId << "date" << date;

        auto result = col.find_one(filter.view());
        if (!result) return 0;

        auto view = result->view();
        if (view["reviews"]) {
            return view["reviews"].get_int32().value;
        }
        return 0;
    } catch (...) {
        std::lock_guard<std::mutex> lock(mutex_);
        return inMemoryReviewFallback_[userId][date];
    }
#else
    std::lock_guard<std::mutex> lock(mutex_);
    return inMemoryReviewFallback_[userId][date];
#endif
}

void MongoAiUsageRepository::incrementReviewUsage(const std::string& userId, const std::string& date) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("ai_usage");
        bsoncxx::builder::stream::document filter;
        filter << "userId" << userId << "date" << date;

        bsoncxx::builder::stream::document updateDoc;
        updateDoc << "$inc" << bsoncxx::builder::stream::open_document
                  << "reviews" << 1
                  << bsoncxx::builder::stream::close_document;

        mongocxx::options::update opts;
        opts.upsert(true);
        col.update_one(filter.view(), updateDoc.view(), opts);
    } catch (...) {
        std::lock_guard<std::mutex> lock(mutex_);
        inMemoryReviewFallback_[userId][date]++;
    }
#else
    std::lock_guard<std::mutex> lock(mutex_);
    inMemoryReviewFallback_[userId][date]++;
#endif
}
