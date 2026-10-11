#include "db/MongoAttemptRepository.h"
#include "db/Database.h"
#include "db/AttemptMapper.h"
#include "db/DatabaseExceptions.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/replace.hpp>
#endif

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of MongoAttemptRepository storing question attempts in MongoDB.

void MongoAttemptRepository::save(const QuestionAttempt& attempt) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("attempts");
        auto doc = AttemptMapper::toBson(attempt);

        bsoncxx::builder::stream::document filter;
        filter << "_id" << attempt.id;

        mongocxx::options::replace opts;
        opts.upsert(true);

        col.replace_one(filter.view(), doc.view(), opts);
    } catch (const std::exception& e) {
        throw QueryException("Failed to save attempt to MongoDB: " + std::string(e.what()));
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<QuestionAttempt> MongoAttemptRepository::findByUserId(const std::string& userId) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<QuestionAttempt> results;
    try {
        auto col = Database::getInstance().getCollection("attempts");
        bsoncxx::builder::stream::document filter;
        filter << "userId" << userId;

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            results.push_back(AttemptMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<QuestionAttempt> MongoAttemptRepository::findByQuestionId(const std::string& questionId) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<QuestionAttempt> results;
    try {
        auto col = Database::getInstance().getCollection("attempts");
        bsoncxx::builder::stream::document filter;
        filter << "questionId" << questionId;

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            results.push_back(AttemptMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<QuestionAttempt> MongoAttemptRepository::findByUserIdAndDate(
    const std::string& userId, const std::string& date) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<QuestionAttempt> results;
    try {
        auto col = Database::getInstance().getCollection("attempts");
        bsoncxx::builder::stream::document filter;
        filter << "userId" << userId << "date" << date;

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            results.push_back(AttemptMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<QuestionAttempt> MongoAttemptRepository::getAll() {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<QuestionAttempt> results;
    try {
        auto col = Database::getInstance().getCollection("attempts");
        auto cursor = col.find({});
        for (const auto& doc : cursor) {
            results.push_back(AttemptMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}
