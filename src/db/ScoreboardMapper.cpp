#include "db/ScoreboardMapper.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#endif

// OOP Concept: Single Responsibility Principle
// Implementation of ScoreboardMapper for ScoreboardEntry objects.

nlohmann::json ScoreboardMapper::toJson(const ScoreboardEntry& entry) {
    nlohmann::json j;
    j["challengeId"] = entry.challengeId;
    j["userId"] = entry.userId;
    j["userName"] = entry.userName;
    j["points"] = entry.points;
    j["executionTimeMs"] = entry.executionTimeMs;
    j["solvedAt"] = entry.solvedAt;
    return j;
}

ScoreboardEntry ScoreboardMapper::fromJson(const nlohmann::json& j) {
    ScoreboardEntry e;
    e.challengeId = j.value("challengeId", "");
    e.userId = j.value("userId", "");
    e.userName = j.value("userName", "");
    e.points = j.value("points", 0);
    e.executionTimeMs = j.value("executionTimeMs", 0);
    e.solvedAt = j.value("solvedAt", "");
    return e;
}

#ifdef AI_INTERVIEWER_HAS_MONGODB
bsoncxx::document::value ScoreboardMapper::toBson(const ScoreboardEntry& entry) {
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::finalize;

    document builder;
    builder << "challengeId" << entry.challengeId
            << "userId" << entry.userId
            << "userName" << entry.userName
            << "points" << entry.points
            << "executionTimeMs" << entry.executionTimeMs
            << "solvedAt" << entry.solvedAt;

    return builder << finalize;
}

ScoreboardEntry ScoreboardMapper::fromBson(const bsoncxx::document::view& doc) {
    ScoreboardEntry e;
    e.challengeId = doc["challengeId"].get_string().value.to_string();
    e.userId = doc["userId"].get_string().value.to_string();
    e.userName = doc["userName"] ? doc["userName"].get_string().value.to_string() : "";
    e.points = doc["points"] ? doc["points"].get_int32().value : 0;
    e.executionTimeMs = doc["executionTimeMs"] ? doc["executionTimeMs"].get_int32().value : 0;
    e.solvedAt = doc["solvedAt"] ? doc["solvedAt"].get_string().value.to_string() : "";
    return e;
}
#endif
