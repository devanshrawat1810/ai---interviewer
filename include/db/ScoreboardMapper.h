#pragma once

#include <nlohmann/json.hpp>
#include "db/IScoreboardRepository.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#endif

// OOP Concept: Single Responsibility Principle
// Translates ScoreboardEntry objects between memory and persistent BSON/JSON formats.
class ScoreboardMapper {
public:
    static nlohmann::json toJson(const ScoreboardEntry& entry);
    static ScoreboardEntry fromJson(const nlohmann::json& j);

#ifdef AI_INTERVIEWER_HAS_MONGODB
    static bsoncxx::document::value toBson(const ScoreboardEntry& entry);
    static ScoreboardEntry fromBson(const bsoncxx::document::view& doc);
#endif
};
