#include "db/UserMapper.h"
#include "core/Admin.h"

#ifdef AI_INTERVIEWER_HAS_MONGODB
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/types.hpp>
#endif

// OOP Concept: Single Responsibility Principle
// Implementation of UserMapper translating User/Student models to/from JSON and BSON.

nlohmann::json UserMapper::toJson(const User& user) {
    nlohmann::json j;
    j["id"] = user.getId();
    j["name"] = user.getName();
    j["email"] = user.getEmail();
    j["passwordHash"] = user.getPasswordHash();
    j["salt"] = user.getSalt();
    j["role"] = user.getRole();
    j["course"] = user.getCourse();
    j["college"] = user.getCollege();
    j["branch"] = user.getBranch();
    j["streak"] = user.getStreak();
    j["longestStreak"] = user.getLongestStreak();
    j["lastActiveDate"] = user.getLastActiveDate();
    j["activityDates"] = user.getActivityDates();
    j["bookmarks"] = user.getBookmarks();

    const auto* student = dynamic_cast<const Student*>(&user);
    if (student) {
        j["badges"] = student->getBadges();
        j["totalScore"] = student->getTotalScore();
        j["dailyChallengesCompleted"] = student->getDailyChallengesCompleted();
        j["solvedEasy"] = student->getSolvedCount(Difficulty::EASY);
        j["solvedMedium"] = student->getSolvedCount(Difficulty::MEDIUM);
        j["solvedHard"] = student->getSolvedCount(Difficulty::HARD);
        
        std::vector<std::string> challenges(student->getSolvedChallenges().begin(),
                                            student->getSolvedChallenges().end());
        j["solvedChallenges"] = challenges;
    }

    return j;
}

std::shared_ptr<User> UserMapper::fromJson(const nlohmann::json& j) {
    std::string role = j.value("role", "student");
    std::string id = j.value("id", "");
    std::string name = j.value("name", "");
    std::string email = j.value("email", "");
    std::string passwordHash = j.value("passwordHash", "");
    std::string salt = j.value("salt", "");
    std::string course = j.value("course", "CSE");
    std::string college = j.value("college", "Unspecified");
    std::string branch = j.value("branch", "Unspecified");
    int streak = j.value("streak", 0);
    int longestStreak = j.value("longestStreak", streak);
    std::string lastActive = j.value("lastActiveDate", "");

    if (role == "admin") {
        auto admin = std::make_shared<Admin>(id, name, email, passwordHash, salt, course, streak, lastActive);
        admin->setCollege(college);
        admin->setBranch(branch);
        admin->setLongestStreak(longestStreak);
        return admin;
    }

    auto student = std::make_shared<Student>(
        id, name, email, passwordHash, salt, course, college, branch, streak, longestStreak, lastActive);

    if (j.contains("badges") && j["badges"].is_array()) {
        student->setBadges(j["badges"].get<std::vector<std::string>>());
    }
    if (j.contains("activityDates") && j["activityDates"].is_array()) {
        student->setActivityDates(j["activityDates"].get<std::vector<std::string>>());
    }
    if (j.contains("bookmarks") && j["bookmarks"].is_array()) {
        student->setBookmarks(j["bookmarks"].get<std::vector<std::string>>());
    }
    if (j.contains("solvedChallenges") && j["solvedChallenges"].is_array()) {
        auto vec = j["solvedChallenges"].get<std::vector<std::string>>();
        student->setSolvedChallenges(std::set<std::string>(vec.begin(), vec.end()));
    }
    int easy = j.value("solvedEasy", 0);
    int med = j.value("solvedMedium", 0);
    int hard = j.value("solvedHard", 0);
    student->setSolvedCounts(easy, med, hard);
    student->setTotalScore(j.value("totalScore", 0));
    student->setDailyChallengesCompleted(j.value("dailyChallengesCompleted", 0));

    return student;
}

#ifdef AI_INTERVIEWER_HAS_MONGODB
bsoncxx::document::value UserMapper::toBson(const User& user) {
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::open_array;
    using bsoncxx::builder::stream::close_array;
    using bsoncxx::builder::stream::finalize;

    document builder;
    builder << "_id" << user.getId()
            << "name" << user.getName()
            << "email" << user.getEmail()
            << "passwordHash" << user.getPasswordHash()
            << "salt" << user.getSalt()
            << "role" << user.getRole()
            << "course" << user.getCourse()
            << "college" << user.getCollege()
            << "branch" << user.getBranch()
            << "streak" << user.getStreak()
            << "longestStreak" << user.getLongestStreak()
            << "lastActive" << user.getLastActiveDate();

    auto datesArr = builder << "activityDates" << open_array;
    for (const auto& d : user.getActivityDates()) {
        datesArr << d;
    }
    datesArr << close_array;

    auto bkmkArr = builder << "bookmarks" << open_array;
    for (const auto& b : user.getBookmarks()) {
        bkmkArr << b;
    }
    bkmkArr << close_array;

    const auto* student = dynamic_cast<const Student*>(&user);
    if (student) {
        auto badgeArr = builder << "badges" << open_array;
        for (const auto& b : student->getBadges()) {
            badgeArr << b;
        }
        badgeArr << close_array;

        builder << "totalScore" << student->getTotalScore()
                << "dailyChallengesCompleted" << student->getDailyChallengesCompleted()
                << "solvedEasy" << student->getSolvedCount(Difficulty::EASY)
                << "solvedMedium" << student->getSolvedCount(Difficulty::MEDIUM)
                << "solvedHard" << student->getSolvedCount(Difficulty::HARD);

        auto challArr = builder << "solvedChallenges" << open_array;
        for (const auto& c : student->getSolvedChallenges()) {
            challArr << c;
        }
        challArr << close_array;
    }

    return builder << finalize;
}

std::shared_ptr<User> UserMapper::fromBson(const bsoncxx::document::view& doc) {
    std::string id = doc["_id"].get_string().value.to_string();
    std::string name = doc["name"].get_string().value.to_string();
    std::string email = doc["email"].get_string().value.to_string();
    std::string passwordHash = doc["passwordHash"].get_string().value.to_string();
    std::string salt = doc["salt"].get_string().value.to_string();
    std::string role = doc["role"] ? doc["role"].get_string().value.to_string() : "student";
    std::string course = doc["course"] ? doc["course"].get_string().value.to_string() : "CSE";
    std::string college = doc["college"] ? doc["college"].get_string().value.to_string() : "Unspecified";
    std::string branch = doc["branch"] ? doc["branch"].get_string().value.to_string() : "Unspecified";
    int streak = doc["streak"] ? doc["streak"].get_int32().value : 0;
    int longestStreak = doc["longestStreak"] ? doc["longestStreak"].get_int32().value : streak;
    std::string lastActive = doc["lastActive"] ? doc["lastActive"].get_string().value.to_string() : "";

    if (role == "admin") {
        auto admin = std::make_shared<Admin>(id, name, email, passwordHash, salt, course, streak, lastActive);
        admin->setCollege(college);
        admin->setBranch(branch);
        admin->setLongestStreak(longestStreak);
        return admin;
    }

    auto student = std::make_shared<Student>(
        id, name, email, passwordHash, salt, course, college, branch, streak, longestStreak, lastActive);

    if (doc["badges"]) {
        std::vector<std::string> badges;
        for (const auto& elem : doc["badges"].get_array().value) {
            badges.push_back(elem.get_string().value.to_string());
        }
        student->setBadges(badges);
    }

    if (doc["activityDates"]) {
        std::vector<std::string> dates;
        for (const auto& elem : doc["activityDates"].get_array().value) {
            dates.push_back(elem.get_string().value.to_string());
        }
        student->setActivityDates(dates);
    }

    if (doc["bookmarks"]) {
        std::vector<std::string> bkmks;
        for (const auto& elem : doc["bookmarks"].get_array().value) {
            bkmks.push_back(elem.get_string().value.to_string());
        }
        student->setBookmarks(bkmks);
    }

    if (doc["solvedChallenges"]) {
        std::set<std::string> challenges;
        for (const auto& elem : doc["solvedChallenges"].get_array().value) {
            challenges.insert(elem.get_string().value.to_string());
        }
        student->setSolvedChallenges(challenges);
    }

    int easy = doc["solvedEasy"] ? doc["solvedEasy"].get_int32().value : 0;
    int med = doc["solvedMedium"] ? doc["solvedMedium"].get_int32().value : 0;
    int hard = doc["solvedHard"] ? doc["solvedHard"].get_int32().value : 0;
    student->setSolvedCounts(easy, med, hard);

    if (doc["totalScore"]) {
        student->setTotalScore(doc["totalScore"].get_int32().value);
    }
    if (doc["dailyChallengesCompleted"]) {
        student->setDailyChallengesCompleted(doc["dailyChallengesCompleted"].get_int32().value);
    }

    return student;
}
#endif
