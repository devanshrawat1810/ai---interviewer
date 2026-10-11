#pragma once

#include <nlohmann/json.hpp>
#include "feedback/FeedbackGenerator.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#endif

// OOP Concept: Single Responsibility Principle
// Translates FeedbackReport entities between domain objects and BSON/JSON formats.
class FeedbackMapper {
public:
    static nlohmann::json toJson(const FeedbackReport& feedback);
    static FeedbackReport fromJson(const nlohmann::json& j);

#ifdef AI_INTERVIEWER_HAS_MONGODB
    static bsoncxx::document::value toBson(const FeedbackReport& feedback);
    static FeedbackReport fromBson(const bsoncxx::document::view& doc);
#endif
};
