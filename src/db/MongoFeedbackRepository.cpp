#include "db/MongoFeedbackRepository.h"
#include "db/Database.h"
#include "db/FeedbackMapper.h"
#include "db/DatabaseExceptions.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/replace.hpp>
#endif

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of MongoFeedbackRepository storing feedback reports in MongoDB.

void MongoFeedbackRepository::save(const FeedbackReport& feedback) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("feedback");
        auto doc = FeedbackMapper::toBson(feedback);

        bsoncxx::builder::stream::document filter;
        filter << "_id" << feedback.sessionId;

        mongocxx::options::replace opts;
        opts.upsert(true);

        col.replace_one(filter.view(), doc.view(), opts);
    } catch (const std::exception& e) {
        throw QueryException("Failed to save feedback report to MongoDB: " + std::string(e.what()));
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::shared_ptr<FeedbackReport> MongoFeedbackRepository::findBySessionId(const std::string& sessionId) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("feedback");
        bsoncxx::builder::stream::document filter;
        filter << "_id" << sessionId;

        auto result = col.find_one(filter.view());
        if (result) {
            return std::make_shared<FeedbackReport>(FeedbackMapper::fromBson(result->view()));
        }
        return nullptr;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<FeedbackReport> MongoFeedbackRepository::findByUserId(const std::string& userId) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<FeedbackReport> results;
    try {
        auto col = Database::getInstance().getCollection("feedback");
        bsoncxx::builder::stream::document filter;
        filter << "userId" << userId;

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            results.push_back(FeedbackMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<FeedbackReport> MongoFeedbackRepository::getAll() {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<FeedbackReport> results;
    try {
        auto col = Database::getInstance().getCollection("feedback");
        auto cursor = col.find({});
        for (const auto& doc : cursor) {
            results.push_back(FeedbackMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}
