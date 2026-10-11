#include "db/MongoScoreboardRepository.h"
#include "db/Database.h"
#include "db/ScoreboardMapper.h"
#include "db/DatabaseExceptions.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/replace.hpp>
#include <mongocxx/options/find.hpp>
#endif

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of MongoScoreboardRepository leveraging MongoDB indexing and sort options.

void MongoScoreboardRepository::recordSolve(const ScoreboardEntry& entry) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("scoreboards");

        bsoncxx::builder::stream::document filter;
        filter << "challengeId" << entry.challengeId
               << "userId" << entry.userId;

        auto existing = col.find_one(filter.view());
        if (existing) {
            auto current = ScoreboardMapper::fromBson(existing->view());
            if (entry.points > current.points ||
               (entry.points == current.points && entry.executionTimeMs < current.executionTimeMs)) {
                auto doc = ScoreboardMapper::toBson(entry);
                mongocxx::options::replace opts;
                opts.upsert(true);
                col.replace_one(filter.view(), doc.view(), opts);
            }
        } else {
            auto doc = ScoreboardMapper::toBson(entry);
            col.insert_one(doc.view());
        }
    } catch (const std::exception& e) {
        throw QueryException("Failed to record solve in MongoDB scoreboard: " + std::string(e.what()));
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<ScoreboardEntry> MongoScoreboardRepository::getAllForChallenge(const std::string& challengeId) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<ScoreboardEntry> results;
    try {
        auto col = Database::getInstance().getCollection("scoreboards");
        bsoncxx::builder::stream::document filter;
        filter << "challengeId" << challengeId;

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            results.push_back(ScoreboardMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<ScoreboardEntry> MongoScoreboardRepository::getTopSolvers(const std::string& challengeId, int limit) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<ScoreboardEntry> results;
    try {
        auto col = Database::getInstance().getCollection("scoreboards");
        bsoncxx::builder::stream::document filter;
        filter << "challengeId" << challengeId;

        bsoncxx::builder::stream::document sortDoc;
        sortDoc << "points" << -1 << "executionTimeMs" << 1;

        mongocxx::options::find opts;
        opts.sort(sortDoc.view());
        opts.limit(limit);

        auto cursor = col.find(filter.view(), opts);
        for (const auto& doc : cursor) {
            results.push_back(ScoreboardMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<ScoreboardEntry> MongoScoreboardRepository::getFastestSolvers(const std::string& challengeId, int limit) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<ScoreboardEntry> results;
    try {
        auto col = Database::getInstance().getCollection("scoreboards");
        bsoncxx::builder::stream::document filter;
        filter << "challengeId" << challengeId;

        bsoncxx::builder::stream::document sortDoc;
        sortDoc << "executionTimeMs" << 1 << "points" << -1;

        mongocxx::options::find opts;
        opts.sort(sortDoc.view());
        opts.limit(limit);

        auto cursor = col.find(filter.view(), opts);
        for (const auto& doc : cursor) {
            results.push_back(ScoreboardMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}
