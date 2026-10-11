#include "db/MongoQuestionRepository.h"
#include "db/Database.h"
#include "db/QuestionMapper.h"
#include "db/DatabaseExceptions.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/replace.hpp>
#endif

// OOP Concept: Interface Implementation & Polymorphism
// Implementation of MongoQuestionRepository querying MongoDB questions collection.

void MongoQuestionRepository::save(const Question& question) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("questions");
        auto doc = QuestionMapper::toBson(question);

        bsoncxx::builder::stream::document filter;
        filter << "_id" << question.getId();

        mongocxx::options::replace opts;
        opts.upsert(true);

        col.replace_one(filter.view(), doc.view(), opts);
    } catch (const std::exception& e) {
        throw QueryException("Failed to save question to MongoDB: " + std::string(e.what()));
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::shared_ptr<Question> MongoQuestionRepository::findById(const std::string& id) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("questions");
        bsoncxx::builder::stream::document filter;
        filter << "_id" << id;

        auto result = col.find_one(filter.view());
        if (result) {
            return QuestionMapper::fromBson(result->view());
        }
        return nullptr;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<std::shared_ptr<Question>> MongoQuestionRepository::findBySubject(const std::string& subject) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<std::shared_ptr<Question>> results;
    try {
        auto col = Database::getInstance().getCollection("questions");
        bsoncxx::builder::stream::document filter;
        filter << "subject" << subject;

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            results.push_back(QuestionMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<std::shared_ptr<Question>> MongoQuestionRepository::findByDifficulty(Difficulty difficulty) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<std::shared_ptr<Question>> results;
    try {
        auto col = Database::getInstance().getCollection("questions");
        bsoncxx::builder::stream::document filter;
        filter << "difficulty" << difficultyToString(difficulty);

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            results.push_back(QuestionMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<std::shared_ptr<Question>> MongoQuestionRepository::findBySubjectAndDifficulty(
    const std::string& subject, Difficulty difficulty) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<std::shared_ptr<Question>> results;
    try {
        auto col = Database::getInstance().getCollection("questions");
        bsoncxx::builder::stream::document filter;
        filter << "subject" << subject
               << "difficulty" << difficultyToString(difficulty);

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            results.push_back(QuestionMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<std::shared_ptr<Question>> MongoQuestionRepository::findByFilters(
    const std::string& /* course */,
    const std::string& subject,
    const std::string& topic,
    Difficulty difficulty) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<std::shared_ptr<Question>> results;
    try {
        auto col = Database::getInstance().getCollection("questions");
        bsoncxx::builder::stream::document filter;
        if (!subject.empty()) {
            filter << "subject" << subject;
        }
        if (!topic.empty()) {
            filter << "topic" << topic;
        }
        filter << "difficulty" << difficultyToString(difficulty);

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            results.push_back(QuestionMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<std::shared_ptr<Question>> MongoQuestionRepository::getAll() {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<std::shared_ptr<Question>> results;
    try {
        auto col = Database::getInstance().getCollection("questions");
        auto cursor = col.find({});
        for (const auto& doc : cursor) {
            results.push_back(QuestionMapper::fromBson(doc));
        }
        return results;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}
