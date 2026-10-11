#pragma once

#include <string>
#include <memory>
#include <mutex>
#include "db/DatabaseExceptions.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/database.hpp>
#endif

// OOP Concept: Singleton Pattern
// Provides a centralized, thread-safe access point to the MongoDB connection and client pool.
// Ensures mongocxx::instance is initialized exactly once per process.
class Database {
private:
    std::string uriString_;
    std::string dbName_;
    bool isConnected_{false};
    mutable std::mutex mutex_;

#ifdef AI_INTERVIEWER_HAS_MONGODB
    static std::unique_ptr<mongocxx::instance> instance_;
    std::unique_ptr<mongocxx::client> client_;
#endif

    // Private constructor enforcing Singleton pattern
    Database();

public:
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // Singleton access point
    static Database& getInstance();

    // Initializes connection from MONGODB_URI and MONGODB_DB environment variables
    bool connect();
    bool connect(const std::string& uri, const std::string& dbName);

    // Health check pinging the database
    bool ping() const;

    // Creates collections and required indexes at application startup
    void initIndexes();

    bool isConnected() const noexcept;
    const std::string& getUri() const noexcept;
    const std::string& getDbName() const noexcept;

#ifdef AI_INTERVIEWER_HAS_MONGODB
    mongocxx::collection getCollection(const std::string& name);
    mongocxx::database getDatabase();
#endif
};
