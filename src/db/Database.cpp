#include "db/Database.h"
#include <iostream>
#include <cstdlib>

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/exception/exception.hpp>

std::unique_ptr<mongocxx::instance> Database::instance_ = nullptr;
#endif

// OOP Concept: Singleton Pattern Implementation
// Controls MongoDB connection lifecycle and enforces single driver instance.

Database::Database() {
    const char* envUri = std::getenv("MONGODB_URI");
    uriString_ = (envUri != nullptr && *envUri != '\0') ? envUri : "mongodb://localhost:27017";

    const char* envDb = std::getenv("MONGODB_DB");
    dbName_ = (envDb != nullptr && *envDb != '\0') ? envDb : "ai_interviewer";
}

Database::~Database() = default;

Database& Database::getInstance() {
    static Database instance;
    return instance;
}

bool Database::connect() {
    return connect(uriString_, dbName_);
}

bool Database::connect(const std::string& uri, const std::string& dbName) {
    std::lock_guard<std::mutex> lock(mutex_);
    uriString_ = uri;
    dbName_ = dbName;

#ifdef AI_INTERVIEWER_HAS_MONGODB
    try {
        if (!instance_) {
            instance_ = std::make_unique<mongocxx::instance>();
        }

        mongocxx::uri mongoUri{uriString_};
        client_ = std::make_unique<mongocxx::client>(mongoUri);

        // Verify connectivity via admin ping
        auto admin = (*client_)["admin"];
        bsoncxx::builder::stream::document pingCmd;
        pingCmd << "ping" << 1;
        admin.run_command(pingCmd.view());

        isConnected_ = true;
        std::cout << "[Database] Successfully connected to MongoDB at: " << uriString_ 
                  << " (db: " << dbName_ << ")\n";

        // Initialize collection indexes
        initIndexes();
        return true;
    } catch (const std::exception& e) {
        isConnected_ = false;
        std::cerr << "[Database] Warning: Could not connect to MongoDB (" << e.what() << ")\n";
        return false;
    }
#else
    isConnected_ = false;
    return false;
#endif
}

bool Database::ping() const {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    if (!isConnected_ || !client_) return false;
    try {
        auto admin = (*client_)["admin"];
        bsoncxx::builder::stream::document pingCmd;
        pingCmd << "ping" << 1;
        admin.run_command(pingCmd.view());
        return true;
    } catch (...) {
        return false;
    }
#else
    return false;
#endif
}

void Database::initIndexes() {
#ifdef AI_INTERVIEWER_HAS_MONGODB
    if (!isConnected_ || !client_) return;

    try {
        auto db = (*client_)[dbName_];

        // 1. users: unique index on email
        {
            auto usersCol = db["users"];
            bsoncxx::builder::stream::document indexDoc;
            indexDoc << "email" << 1;
            mongocxx::options::index indexOpts{};
            indexOpts.unique(true);
            usersCol.create_index(indexDoc.view(), indexOpts);
        }

        // 2. questions: compound index on (course, subject, difficulty)
        {
            auto questionsCol = db["questions"];
            bsoncxx::builder::stream::document indexDoc;
            indexDoc << "course" << 1
                     << "subject" << 1
                     << "difficulty" << 1;
            questionsCol.create_index(indexDoc.view());
        }

        // 3. attempts: index on (userId, date) and on questionId
        {
            auto attemptsCol = db["attempts"];
            bsoncxx::builder::stream::document userDateDoc;
            userDateDoc << "userId" << 1 << "date" << 1;
            attemptsCol.create_index(userDateDoc.view());

            bsoncxx::builder::stream::document qDoc;
            qDoc << "questionId" << 1;
            attemptsCol.create_index(qDoc.view());
        }

        // 4. interviews: index on userId
        {
            auto interviewsCol = db["interviews"];
            bsoncxx::builder::stream::document userDoc;
            userDoc << "userId" << 1;
            interviewsCol.create_index(userDoc.view());
        }

        // 5. feedback: index on userId
        {
            auto feedbackCol = db["feedback"];
            bsoncxx::builder::stream::document userDoc;
            userDoc << "userId" << 1;
            feedbackCol.create_index(userDoc.view());
        }

        // 6. scoreboards: index on challengeId and userId
        {
            auto scoreboardsCol = db["scoreboards"];
            bsoncxx::builder::stream::document scoreDoc;
            scoreDoc << "challengeId" << 1 << "userId" << 1;
            scoreboardsCol.create_index(scoreDoc.view());
        }
    } catch (const std::exception& e) {
        std::cerr << "[Database] Warning: Failed to ensure indexes: " << e.what() << "\n";
    }
#endif
}

bool Database::isConnected() const noexcept {
    return isConnected_;
}

const std::string& Database::getUri() const noexcept {
    return uriString_;
}

const std::string& Database::getDbName() const noexcept {
    return dbName_;
}

#ifdef AI_INTERVIEWER_HAS_MONGODB
mongocxx::collection Database::getCollection(const std::string& name) {
    if (!isConnected_ || !client_) {
        throw ConnectionException("Attempted to access collection '" + name + "' while database is disconnected.");
    }
    return (*client_)[dbName_][name];
}

mongocxx::database Database::getDatabase() {
    if (!isConnected_ || !client_) {
        throw ConnectionException("Attempted to access database while disconnected.");
    }
    return (*client_)[dbName_];
}
#endif
