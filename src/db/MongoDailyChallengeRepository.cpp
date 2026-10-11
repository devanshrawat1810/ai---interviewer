#include "db/MongoDailyChallengeRepository.h"
#include "db/Database.h"
#include "db/DatabaseExceptions.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/replace.hpp>
#include <mongocxx/options/find.hpp>
#endif

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of MongoDailyChallengeRepository.

void MongoDailyChallengeRepository::save(const DailyChallengeRecord& record) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("daily_challenges");
        bsoncxx::builder::stream::document filter;
        filter << "date" << record.date << "course" << record.course;

        bsoncxx::builder::stream::document doc;
        doc << "_id" << record.id
            << "date" << record.date
            << "course" << record.course
            << "questionId" << record.questionId;

        mongocxx::options::replace opts;
        opts.upsert(true);
        col.replace_one(filter.view(), doc.view(), opts);
    } catch (const std::exception& e) {
        throw QueryException("Failed to save daily challenge to MongoDB: " + std::string(e.what()));
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::shared_ptr<DailyChallengeRecord> MongoDailyChallengeRepository::findByDateAndCourse(
    const std::string& date, const std::string& course) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("daily_challenges");
        bsoncxx::builder::stream::document filter;
        filter << "date" << date << "course" << course;

        auto result = col.find_one(filter.view());
        if (result) {
            auto view = result->view();
            auto rec = std::make_shared<DailyChallengeRecord>();
            rec->id = view["_id"].get_string().value.to_string();
            rec->date = view["date"].get_string().value.to_string();
            rec->course = view["course"].get_string().value.to_string();
            rec->questionId = view["questionId"].get_string().value.to_string();
            return rec;
        }
        return nullptr;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<std::string> MongoDailyChallengeRepository::getPast30DaysQuestionIds(
    const std::string& /* currentDate */, const std::string& course) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<std::string> questionIds;
    try {
        auto col = Database::getInstance().getCollection("daily_challenges");
        bsoncxx::builder::stream::document filter;
        filter << "course" << course;

        bsoncxx::builder::stream::document sortDoc;
        sortDoc << "date" << -1;

        mongocxx::options::find opts;
        opts.sort(sortDoc.view());
        opts.limit(30);

        auto cursor = col.find(filter.view(), opts);
        for (const auto& doc : cursor) {
            questionIds.push_back(doc["questionId"].get_string().value.to_string());
        }
        return questionIds;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<DailyChallengeRecord> MongoDailyChallengeRepository::getAll() {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<DailyChallengeRecord> results;
    try {
        auto col = Database::getInstance().getCollection("daily_challenges");
        auto cursor = col.find({});
        for (const auto& doc : cursor) {
            DailyChallengeRecord r;
            r.id = doc["_id"].get_string().value.to_string();
            r.date = doc["date"].get_string().value.to_string();
            r.course = doc["course"].get_string().value.to_string();
            r.questionId = doc["questionId"].get_string().value.to_string();
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
