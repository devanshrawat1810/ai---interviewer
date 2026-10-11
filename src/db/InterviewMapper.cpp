#include "db/InterviewMapper.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#endif

// OOP Concept: Single Responsibility Principle
// Implementation of InterviewMapper for StoredInterviewSession objects.

nlohmann::json InterviewMapper::toJson(const StoredInterviewSession& session) {
    nlohmann::json j;
    j["id"] = session.id;
    j["userId"] = session.userId;
    j["subjectId"] = session.subjectId;
    j["mode"] = session.mode;
    j["totalScore"] = session.totalScore;
    j["maxScore"] = session.maxScore;
    j["questionsAttempted"] = session.questionsAttempted;
    j["correctCount"] = session.correctCount;
    j["date"] = session.date;
    return j;
}

StoredInterviewSession InterviewMapper::fromJson(const nlohmann::json& j) {
    StoredInterviewSession s;
    s.id = j.value("id", "");
    s.userId = j.value("userId", "");
    s.subjectId = j.value("subjectId", "");
    s.mode = j.value("mode", "mock");
    s.totalScore = j.value("totalScore", 0);
    s.maxScore = j.value("maxScore", 0);
    s.questionsAttempted = j.value("questionsAttempted", 0);
    s.correctCount = j.value("correctCount", 0);
    s.date = j.value("date", "");
    return s;
}

#ifdef AI_INTERVIEWER_HAS_MONGODB
bsoncxx::document::value InterviewMapper::toBson(const StoredInterviewSession& session) {
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::finalize;

    document builder;
    builder << "_id" << session.id
            << "userId" << session.userId
            << "subjectId" << session.subjectId
            << "mode" << session.mode
            << "totalScore" << session.totalScore
            << "maxScore" << session.maxScore
            << "questionsAttempted" << session.questionsAttempted
            << "correctCount" << session.correctCount
            << "date" << session.date;

    return builder << finalize;
}

StoredInterviewSession InterviewMapper::fromBson(const bsoncxx::document::view& doc) {
    StoredInterviewSession s;
    s.id = doc["_id"].get_string().value.to_string();
    s.userId = doc["userId"].get_string().value.to_string();
    s.subjectId = doc["subjectId"] ? doc["subjectId"].get_string().value.to_string() : "";
    s.mode = doc["mode"] ? doc["mode"].get_string().value.to_string() : "mock";
    s.totalScore = doc["totalScore"] ? doc["totalScore"].get_int32().value : 0;
    s.maxScore = doc["maxScore"] ? doc["maxScore"].get_int32().value : 0;
    s.questionsAttempted = doc["questionsAttempted"] ? doc["questionsAttempted"].get_int32().value : 0;
    s.correctCount = doc["correctCount"] ? doc["correctCount"].get_int32().value : 0;
    s.date = doc["date"] ? doc["date"].get_string().value.to_string() : "";
    return s;
}
#endif
