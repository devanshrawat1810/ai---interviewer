#include "db/MongoQuestionFeedbackRepository.h"
#include "db/Database.h"
#include "db/DatabaseExceptions.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/replace.hpp>
#endif

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of MongoQuestionFeedbackRepository.

void MongoQuestionFeedbackRepository::saveReport(const QuestionReport& report) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("question_reports");
        bsoncxx::builder::stream::document builder;
        builder << "_id" << report.id
                << "questionId" << report.questionId
                << "userId" << report.userId
                << "reason" << report.reason
                << "reportedAt" << report.reportedAt;
        col.insert_one(builder.view());
    } catch (const std::exception& e) {
        throw QueryException("Failed to save question report in MongoDB: " + std::string(e.what()));
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<QuestionReport> MongoQuestionFeedbackRepository::getAllReports() {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<QuestionReport> results;
    try {
        auto col = Database::getInstance().getCollection("question_reports");
        auto cursor = col.find({});
        for (const auto& doc : cursor) {
            QuestionReport r;
            r.id = doc["_id"].get_string().value.to_string();
            r.questionId = doc["questionId"].get_string().value.to_string();
            r.userId = doc["userId"].get_string().value.to_string();
            r.reason = doc["reason"].get_string().value.to_string();
            r.reportedAt = doc["reportedAt"].get_string().value.to_string();
            results.push_back(r);
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

void MongoQuestionFeedbackRepository::saveDifficultyFeedback(const DifficultyFeedback& fb) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("difficulty_feedback");
        bsoncxx::builder::stream::document builder;
        builder << "_id" << fb.id
                << "questionId" << fb.questionId
                << "userId" << fb.userId
                << "feedback" << fb.feedback
                << "submittedAt" << fb.submittedAt;
        col.insert_one(builder.view());
    } catch (const std::exception& e) {
        throw QueryException("Failed to save difficulty feedback in MongoDB: " + std::string(e.what()));
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<DifficultyFeedback> MongoQuestionFeedbackRepository::getDifficultyFeedbackForQuestion(const std::string& questionId) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<DifficultyFeedback> results;
    try {
        auto col = Database::getInstance().getCollection("difficulty_feedback");
        bsoncxx::builder::stream::document filter;
        filter << "questionId" << questionId;

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            DifficultyFeedback fb;
            fb.id = doc["_id"].get_string().value.to_string();
            fb.questionId = doc["questionId"].get_string().value.to_string();
            fb.userId = doc["userId"].get_string().value.to_string();
            fb.feedback = doc["feedback"].get_string().value.to_string();
            fb.submittedAt = doc["submittedAt"].get_string().value.to_string();
            results.push_back(fb);
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<DifficultyFeedback> MongoQuestionFeedbackRepository::getAllDifficultyFeedback() {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<DifficultyFeedback> results;
    try {
        auto col = Database::getInstance().getCollection("difficulty_feedback");
        auto cursor = col.find({});
        for (const auto& doc : cursor) {
            DifficultyFeedback fb;
            fb.id = doc["_id"].get_string().value.to_string();
            fb.questionId = doc["questionId"].get_string().value.to_string();
            fb.userId = doc["userId"].get_string().value.to_string();
            fb.feedback = doc["feedback"].get_string().value.to_string();
            fb.submittedAt = doc["submittedAt"].get_string().value.to_string();
            results.push_back(fb);
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}
