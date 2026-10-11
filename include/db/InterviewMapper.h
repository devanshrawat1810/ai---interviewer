#pragma once

#include <nlohmann/json.hpp>
#include "db/IInterviewRepository.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#endif

// OOP Concept: Single Responsibility Principle
// Translates StoredInterviewSession objects between memory and persistent BSON/JSON formats.
class InterviewMapper {
public:
    static nlohmann::json toJson(const StoredInterviewSession& session);
    static StoredInterviewSession fromJson(const nlohmann::json& j);

#ifdef AI_INTERVIEWER_HAS_MONGODB
    static bsoncxx::document::value toBson(const StoredInterviewSession& session);
    static StoredInterviewSession fromBson(const bsoncxx::document::view& doc);
#endif
};
