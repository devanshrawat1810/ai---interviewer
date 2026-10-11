#include "db/AttemptMapper.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#endif

// OOP Concept: Single Responsibility Principle
// Implementation of AttemptMapper for QuestionAttempt records.

nlohmann::json AttemptMapper::toJson(const QuestionAttempt& attempt) {
    nlohmann::json j;
    j["id"] = attempt.id;
    j["userId"] = attempt.userId;
    j["questionId"] = attempt.questionId;
    j["subject"] = attempt.subject;
    j["difficulty"] = difficultyToString(attempt.difficulty);
    j["userResponse"] = attempt.userResponse;
    j["isCorrect"] = attempt.isCorrect;
    j["timeTakenSeconds"] = attempt.timeTakenSeconds;
    j["pointsEarned"] = attempt.pointsEarned;
    j["date"] = attempt.date;
    return j;
}

QuestionAttempt AttemptMapper::fromJson(const nlohmann::json& j) {
    QuestionAttempt a;
    a.id = j.value("id", "");
    a.userId = j.value("userId", "");
    a.questionId = j.value("questionId", "");
    a.subject = j.value("subject", "");
    a.difficulty = difficultyFromString(j.value("difficulty", "EASY"));
    a.userResponse = j.value("userResponse", "");
    a.isCorrect = j.value("isCorrect", false);
    a.timeTakenSeconds = j.value("timeTakenSeconds", 0);
    a.pointsEarned = j.value("pointsEarned", 0);
    a.date = j.value("date", "");
    return a;
}

#ifdef AI_INTERVIEWER_HAS_MONGODB
bsoncxx::document::value AttemptMapper::toBson(const QuestionAttempt& attempt) {
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::finalize;

    document builder;
    builder << "_id" << attempt.id
            << "userId" << attempt.userId
            << "questionId" << attempt.questionId
            << "subject" << attempt.subject
            << "difficulty" << difficultyToString(attempt.difficulty)
            << "userResponse" << attempt.userResponse
            << "isCorrect" << attempt.isCorrect
            << "timeTakenSeconds" << attempt.timeTakenSeconds
            << "pointsEarned" << attempt.pointsEarned
            << "date" << attempt.date;

    return builder << finalize;
}

QuestionAttempt AttemptMapper::fromBson(const bsoncxx::document::view& doc) {
    QuestionAttempt a;
    a.id = doc["_id"].get_string().value.to_string();
    a.userId = doc["userId"].get_string().value.to_string();
    a.questionId = doc["questionId"].get_string().value.to_string();
    a.subject = doc["subject"] ? doc["subject"].get_string().value.to_string() : "";
    a.difficulty = difficultyFromString(doc["difficulty"] ? doc["difficulty"].get_string().value.to_string() : "EASY");
    a.userResponse = doc["userResponse"] ? doc["userResponse"].get_string().value.to_string() : "";
    a.isCorrect = doc["isCorrect"] ? doc["isCorrect"].get_bool().value : false;
    a.timeTakenSeconds = doc["timeTakenSeconds"] ? doc["timeTakenSeconds"].get_int32().value : 0;
    a.pointsEarned = doc["pointsEarned"] ? doc["pointsEarned"].get_int32().value : 0;
    a.date = doc["date"] ? doc["date"].get_string().value.to_string() : "";
    return a;
}
#endif
