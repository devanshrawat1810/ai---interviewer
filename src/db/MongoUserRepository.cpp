#include "db/MongoUserRepository.h"
#include "db/Database.h"
#include "db/UserMapper.h"
#include "db/DatabaseExceptions.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/replace.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#endif

// OOP Concept: Interface Implementation & Polymorphism
// MongoDB user repository implementation satisfying IUserRepository contract.

void MongoUserRepository::save(const User& user) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("users");
        auto doc = UserMapper::toBson(user);

        bsoncxx::builder::stream::document filter;
        filter << "_id" << user.getId();

        mongocxx::options::replace opts;
        opts.upsert(true);

        col.replace_one(filter.view(), doc.view(), opts);
    } catch (const mongocxx::bulk_write_exception& e) {
        // Code 11000 corresponds to MongoDB duplicate key error (unique index on email)
        std::string msg = e.what();
        if (msg.find("11000") != std::string::npos || msg.find("duplicate key") != std::string::npos) {
            throw DuplicateKeyException("User with email '" + user.getEmail() + "' already exists.");
        }
        throw QueryException("Failed to save user to MongoDB: " + msg);
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::shared_ptr<User> MongoUserRepository::findById(const std::string& id) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("users");
        bsoncxx::builder::stream::document filter;
        filter << "_id" << id;

        auto result = col.find_one(filter.view());
        if (result) {
            return UserMapper::fromBson(result->view());
        }
        return nullptr;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::shared_ptr<User> MongoUserRepository::findByEmail(const std::string& email) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("users");
        bsoncxx::builder::stream::document filter;
        filter << "email" << email;

        auto result = col.find_one(filter.view());
        if (result) {
            return UserMapper::fromBson(result->view());
        }
        return nullptr;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

std::vector<std::shared_ptr<Student>> MongoUserRepository::getAllStudents() {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    std::vector<std::shared_ptr<Student>> students;
    try {
        auto col = Database::getInstance().getCollection("users");
        bsoncxx::builder::stream::document filter;
        filter << "role" << "student";

        auto cursor = col.find(filter.view());
        for (const auto& doc : cursor) {
            auto user = UserMapper::fromBson(doc);
            if (user && !user->isAdmin()) {
                auto student = std::dynamic_pointer_cast<Student>(user);
                if (student) {
                    students.push_back(student);
                }
            }
        }
        return students;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}

bool MongoUserRepository::deleteById(const std::string& id) {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        auto col = Database::getInstance().getCollection("users");
        bsoncxx::builder::stream::document filter;
        filter << "_id" << id;

        auto result = col.delete_one(filter.view());
        return result && result->deleted_count() > 0;
    } catch (const std::exception& e) {
        throw QueryException(e.what());
    }
#else
    throw ConnectionException("MongoDB driver not compiled in binary.");
#endif
}
