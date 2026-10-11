#include "db/MongoInterviewRepository.h"
#include "db/Database.h"
#include "db/InterviewMapper.h"
#include "db/DatabaseExceptions.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/replace.hpp>
#endif

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of MongoInterviewRepository storing interview sessions in MongoDB.

void MongoInterviewRepository::save(const StoredInterviewSession& session) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("interviews");
        auto doc = InterviewMapper::toBson(session);

        bsoncxx::builder::stream::document filter;
        filter << "_id" << session.id;

        mongocxx::options::replace opts;
        opts.upsert(true);

        col.replace_one(filter.view(), doc.view(), opts);
    } catch (const std::exception& e) {
        throw QueryException("Failed to save interview session to MongoDB: " + std::string(e.what()));
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::shared_ptr<StoredInterviewSession> MongoInterviewRepository::findById(const std::string& id) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("interviews");
        bsoncxx::builder::stream::document filter;
        filter << "_id" << id;

        auto result = col.find_one(filter.view());
        if (result) {
            return std::make_shared<StoredInterviewSession>(InterviewMapper::fromBson(result->view()));
        }
        return nullptr;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<StoredInterviewSession> MongoInterviewRepository::findByUserId(const std::string& userId) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<StoredInterviewSession> results;
    try {
        auto col = Database::getInstance().getCollection("interviews");
        bsoncxx::builder::stream::document filter;
        filter << "userId" << userId;

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            results.push_back(InterviewMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<StoredInterviewSession> MongoInterviewRepository::getAll() {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<StoredInterviewSession> results;
    try {
        auto col = Database::getInstance().getCollection("interviews");
        auto cursor = col.find({});
        for (const auto& doc : cursor) {
            results.push_back(InterviewMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}
