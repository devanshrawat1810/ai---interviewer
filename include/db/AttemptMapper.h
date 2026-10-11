#pragma once

#include <nlohmann/json.hpp>
#include "db/IAttemptRepository.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#endif

// OOP Concept: Single Responsibility Principle
// Translates QuestionAttempt records between memory representations and persistent BSON/JSON formats.
class AttemptMapper {
public:
    static nlohmann::json toJson(const QuestionAttempt& attempt);
    static QuestionAttempt fromJson(const nlohmann::json& j);

#ifdef AI_INTERVIEWER_HAS_MONGODB
    static bsoncxx::document::value toBson(const QuestionAttempt& attempt);
    static QuestionAttempt fromBson(const bsoncxx::document::view& doc);
#endif
};
