#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include "core/User.h"
#include "core/Student.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>
#endif

// OOP Concept: Single Responsibility Principle
// Isolates serialization and deserialization logic between domain User/Student objects
// and JSON / BSON representations. Keeps repository classes focused strictly on persistence.
class UserMapper {
public:
    static nlohmann::json toJson(const User& user);
    static std::shared_ptr<User> fromJson(const nlohmann::json& j);

#ifdef AI_INTERVIEWER_HAS_MONGODB
    static bsoncxx::document::value toBson(const User& user);
    static std::shared_ptr<User> fromBson(const bsoncxx::document::view& doc);
#endif
};
