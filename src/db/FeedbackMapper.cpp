#include "db/FeedbackMapper.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/types.hpp>
#endif

// OOP Concept: Single Responsibility Principle
// Implementation of FeedbackMapper for FeedbackReport structures.

nlohmann::json FeedbackMapper::toJson(const FeedbackReport& feedback) {
    nlohmann::json j;
    j["sessionId"] = feedback.sessionId;
    j["userId"] = feedback.userId;
    j["subjectId"] = feedback.subjectId;
    j["totalScore"] = feedback.totalScore;
    j["maxScore"] = feedback.maxScore;
    j["percentage"] = feedback.percentage;
    j["strongestTopic"] = feedback.strongestTopic;
    j["weakestTopic"] = feedback.weakestTopic;
    j["suggestions"] = feedback.suggestions;
    j["aiEvaluation"] = feedback.aiEvaluation;
    return j;
}

FeedbackReport FeedbackMapper::fromJson(const nlohmann::json& j) {
    FeedbackReport f;
    f.sessionId = j.value("sessionId", "");
    f.userId = j.value("userId", "");
    f.subjectId = j.value("subjectId", "");
    f.totalScore = j.value("totalScore", 0);
    f.maxScore = j.value("maxScore", 0);
    f.percentage = j.value("percentage", 0.0);
    f.strongestTopic = j.value("strongestTopic", "");
    f.weakestTopic = j.value("weakestTopic", "");
    if (j.contains("suggestions") && j["suggestions"].is_array()) {
        f.suggestions = j["suggestions"].get<std::vector<std::string>>();
    }
    f.aiEvaluation = j.value("aiEvaluation", "");
    return f;
}

#ifdef AI_INTERVIEWER_HAS_MONGODB
bsoncxx::document::value FeedbackMapper::toBson(const FeedbackReport& feedback) {
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::open_array;
    using bsoncxx::builder::stream::close_array;
    using bsoncxx::builder::stream::finalize;

    document builder;
    builder << "_id" << feedback.sessionId
            << "userId" << feedback.userId
            << "subjectId" << feedback.subjectId
            << "totalScore" << feedback.totalScore
            << "maxScore" << feedback.maxScore
            << "percentage" << feedback.percentage
            << "strongestTopic" << feedback.strongestTopic
            << "weakestTopic" << feedback.weakestTopic
            << "aiEvaluation" << feedback.aiEvaluation;

    auto suggArr = builder << "suggestions" << open_array;
    for (const auto& s : feedback.suggestions) {
        suggArr << s;
    }
    suggArr << close_array;

    return builder << finalize;
}

FeedbackReport FeedbackMapper::fromBson(const bsoncxx::document::view& doc) {
    FeedbackReport f;
    f.sessionId = doc["_id"].get_string().value.to_string();
    f.userId = doc["userId"].get_string().value.to_string();
    f.subjectId = doc["subjectId"] ? doc["subjectId"].get_string().value.to_string() : "";
    f.totalScore = doc["totalScore"] ? doc["totalScore"].get_int32().value : 0;
    f.maxScore = doc["maxScore"] ? doc["maxScore"].get_int32().value : 0;
    f.percentage = doc["percentage"] ? doc["percentage"].get_double().value : 0.0;
    f.strongestTopic = doc["strongestTopic"] ? doc["strongestTopic"].get_string().value.to_string() : "";
    f.weakestTopic = doc["weakestTopic"] ? doc["weakestTopic"].get_string().value.to_string() : "";
    f.aiEvaluation = doc["aiEvaluation"] ? doc["aiEvaluation"].get_string().value.to_string() : "";

    if (doc["suggestions"]) {
        for (const auto& elem : doc["suggestions"].get_array().value) {
            f.suggestions.push_back(elem.get_string().value.to_string());
        }
    }

    return f;
}
#endif
