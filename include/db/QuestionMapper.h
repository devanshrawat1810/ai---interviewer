#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include "core/Question.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#endif

// OOP Concept: Single Responsibility Principle & Factory Deserialization
// Maps Question hierarchies (MCQQuestion, TextQuestion, CodingQuestion) between domain objects
// and persistent JSON/BSON structures.
class QuestionMapper {
public:
    static nlohmann::json toJson(const Question& question);
    static std::shared_ptr<Question> fromJson(const nlohmann::json& j);

#ifdef AI_INTERVIEWER_HAS_MONGODB
    static bsoncxx::document::value toBson(const Question& question);
    static std::shared_ptr<Question> fromBson(const bsoncxx::document::view& doc);
#endif
};
