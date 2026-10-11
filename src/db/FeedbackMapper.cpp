#include "db/FeedbackMapper.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/types.hpp>
#endif

// OOP Concept: Single Responsibility Principle
// Implementation of FeedbackMapper for FeedbackReport structures with Phase 4 AI fields.

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

    // Phase 4 additions
    j["isAiGenerated"] = feedback.isAiGenerated;
    j["overallSummary"] = feedback.overallSummary;
    j["strengths"] = feedback.strengths;
    j["weaknesses"] = feedback.weaknesses;
    j["communicationClarity"] = feedback.communicationClarity;
    j["answerStructure"] = feedback.answerStructure;
    j["confidenceCues"] = feedback.confidenceCues;
    j["notice"] = feedback.notice;
    j["perTopicNotes"] = feedback.perTopicNotes;
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

    // Phase 4 additions
    f.isAiGenerated = j.value("isAiGenerated", false);
    f.overallSummary = j.value("overallSummary", "");
    if (j.contains("strengths") && j["strengths"].is_array()) {
        f.strengths = j["strengths"].get<std::vector<std::string>>();
    }
    if (j.contains("weaknesses") && j["weaknesses"].is_array()) {
        f.weaknesses = j["weaknesses"].get<std::vector<std::string>>();
    }
    f.communicationClarity = j.value("communicationClarity", 3);
    f.answerStructure = j.value("answerStructure", 3);
    f.confidenceCues = j.value("confidenceCues", 3);
    f.notice = j.value("notice", "");
    if (j.contains("perTopicNotes") && j["perTopicNotes"].is_object()) {
        f.perTopicNotes = j["perTopicNotes"].get<std::map<std::string, std::string>>();
    }
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
            << "aiEvaluation" << feedback.aiEvaluation
            << "isAiGenerated" << feedback.isAiGenerated
            << "overallSummary" << feedback.overallSummary
            << "communicationClarity" << feedback.communicationClarity
            << "answerStructure" << feedback.answerStructure
            << "confidenceCues" << feedback.confidenceCues
            << "notice" << feedback.notice;

    auto suggArr = builder << "suggestions" << open_array;
    for (const auto& s : feedback.suggestions) {
        suggArr << s;
    }
    suggArr << close_array;

    auto strArr = builder << "strengths" << open_array;
    for (const auto& s : feedback.strengths) {
        strArr << s;
    }
    strArr << close_array;

    auto wArr = builder << "weaknesses" << open_array;
    for (const auto& w : feedback.weaknesses) {
        wArr << w;
    }
    wArr << close_array;

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

    f.isAiGenerated = doc["isAiGenerated"] ? doc["isAiGenerated"].get_bool().value : false;
    f.overallSummary = doc["overallSummary"] ? doc["overallSummary"].get_string().value.to_string() : "";
    f.communicationClarity = doc["communicationClarity"] ? doc["communicationClarity"].get_int32().value : 3;
    f.answerStructure = doc["answerStructure"] ? doc["answerStructure"].get_int32().value : 3;
    f.confidenceCues = doc["confidenceCues"] ? doc["confidenceCues"].get_int32().value : 3;
    f.notice = doc["notice"] ? doc["notice"].get_string().value.to_string() : "";

    if (doc["suggestions"]) {
        for (const auto& elem : doc["suggestions"].get_array().value) {
            f.suggestions.push_back(elem.get_string().value.to_string());
        }
    }
    if (doc["strengths"]) {
        for (const auto& elem : doc["strengths"].get_array().value) {
            f.strengths.push_back(elem.get_string().value.to_string());
        }
    }
    if (doc["weaknesses"]) {
        for (const auto& elem : doc["weaknesses"].get_array().value) {
            f.weaknesses.push_back(elem.get_string().value.to_string());
        }
    }

    return f;
}
#endif
