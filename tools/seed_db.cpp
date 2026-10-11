#include "db/Database.h"
#include "db/UserMapper.h"
#include "db/QuestionMapper.h"
#include "db/AttemptMapper.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/replace.hpp>
#endif

namespace fs = std::filesystem;

// Migration and Seed Tool for populating MongoDB from JSON files (Phase 3).
// Idempotent: executes upserts keyed on stable _id so duplicate runs are completely safe.

struct SeedSummary {
    int inserted{0};
    int updated{0};
    int skipped{0};
    int failed{0};
};

int main(int argc, char* argv[]) {
    std::cout << "=======================================================\n";
    std::cout << "       AI Interviewer: MongoDB Database Seed Tool      \n";
    std::cout << "=======================================================\n";

    std::string dataDir = "data";
    if (argc > 1) {
        dataDir = argv[1];
    }

    auto& db = Database::getInstance();
    if (!db.connect()) {
        std::cerr << "Error: Could not connect to MongoDB at: " << db.getUri() << "\n";
        std::cerr << "Ensure MongoDB is running (e.g. 'docker compose up -d') and MONGODB_URI is configured.\n";
        return 1;
    }

#ifdef AI_INTERVIEWER_HAS_MONGODB
    auto mongoDb = db.getDatabase();
    SeedSummary totalSummary;

    // 1. Seed Questions from data/questions/*.json
    std::cout << "\n[1/3] Seeding questions from: " << dataDir << "/questions/ ...\n";
    std::string qDir = dataDir + "/questions";
    if (fs::exists(qDir)) {
        auto col = mongoDb["questions"];
        for (const auto& entry : fs::directory_iterator(qDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::ifstream in(entry.path());
                if (!in) continue;

                nlohmann::json root;
                try { in >> root; } catch (...) { continue; }

                auto processQuestion = [&](const nlohmann::json& item) {
                    try {
                        auto q = QuestionMapper::fromJson(item);
                        if (!q) {
                            totalSummary.skipped++;
                            return;
                        }

                        bsoncxx::builder::stream::document filter;
                        filter << "_id" << q->getId();

                        auto existing = col.find_one(filter.view());
                        auto doc = QuestionMapper::toBson(*q);

                        mongocxx::options::replace opts;
                        opts.upsert(true);
                        auto res = col.replace_one(filter.view(), doc.view(), opts);

                        if (res && res->upserted_id()) totalSummary.inserted++;
                        else if (existing) totalSummary.updated++;
                        else totalSummary.inserted++;
                    } catch (const std::exception& e) {
                        std::cerr << "Failed to seed question: " << e.what() << "\n";
                        totalSummary.failed++;
                    }
                };

                if (root.is_array()) {
                    for (const auto& item : root) processQuestion(item);
                } else if (root.is_object() && root.contains("questions") && root["questions"].is_array()) {
                    for (const auto& item : root["questions"]) processQuestion(item);
                } else if (root.is_object()) {
                    processQuestion(root);
                }
            }
        }
    }

    // 2. Seed Users from data/users.json
    std::cout << "[2/3] Seeding users from: " << dataDir << "/users.json ...\n";
    std::string userFile = dataDir + "/users.json";
    if (fs::exists(userFile)) {
        auto col = mongoDb["users"];
        std::ifstream in(userFile);
        if (in) {
            nlohmann::json usersArr;
            try {
                in >> usersArr;
                if (usersArr.is_array()) {
                    for (const auto& item : usersArr) {
                        try {
                            auto u = UserMapper::fromJson(item);
                            if (!u) {
                                totalSummary.skipped++;
                                continue;
                            }

                            bsoncxx::builder::stream::document filter;
                            filter << "_id" << u->getId();

                            auto existing = col.find_one(filter.view());
                            auto doc = UserMapper::toBson(*u);

                            mongocxx::options::replace opts;
                            opts.upsert(true);
                            auto res = col.replace_one(filter.view(), doc.view(), opts);

                            if (res && res->upserted_id()) totalSummary.inserted++;
                            else if (existing) totalSummary.updated++;
                            else totalSummary.inserted++;
                        } catch (const std::exception& e) {
                            std::cerr << "Failed to seed user: " << e.what() << "\n";
                            totalSummary.failed++;
                        }
                    }
                }
            } catch (...) {}
        }
    }

    // 3. Seed Attempts from data/attempts.json
    std::cout << "[3/3] Seeding attempts from: " << dataDir << "/attempts.json ...\n";
    std::string attFile = dataDir + "/attempts.json";
    if (fs::exists(attFile)) {
        auto col = mongoDb["attempts"];
        std::ifstream in(attFile);
        if (in) {
            nlohmann::json attArr;
            try {
                in >> attArr;
                if (attArr.is_array()) {
                    for (const auto& item : attArr) {
                        try {
                            auto att = AttemptMapper::fromJson(item);
                            bsoncxx::builder::stream::document filter;
                            filter << "_id" << att.id;

                            auto existing = col.find_one(filter.view());
                            auto doc = AttemptMapper::toBson(att);

                            mongocxx::options::replace opts;
                            opts.upsert(true);
                            auto res = col.replace_one(filter.view(), doc.view(), opts);

                            if (res && res->upserted_id()) totalSummary.inserted++;
                            else if (existing) totalSummary.updated++;
                            else totalSummary.inserted++;
                        } catch (const std::exception& e) {
                            std::cerr << "Failed to seed attempt: " << e.what() << "\n";
                            totalSummary.failed++;
                        }
                    }
                }
            } catch (...) {}
        }
    }

    std::cout << "\n================ SEED SUMMARY ================\n";
    std::cout << "  Records Inserted: " << totalSummary.inserted << "\n";
    std::cout << "  Records Updated:  " << totalSummary.updated << "\n";
    std::cout << "  Records Skipped:  " << totalSummary.skipped << "\n";
    std::cout << "  Records Failed:   " << totalSummary.failed << "\n";
    std::cout << "==============================================\n";
    std::cout << "Database migration and seed completed successfully.\n";

#else
    std::cout << "Note: Built without native mongocxx driver.\n";
#endif

    return 0;
}
