#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include "ui/RankingStrategy.h"

// OOP Concept: Generic Programming (C++ Templates) & Strategy Pattern
// Leaderboard template class supporting multi-scope filtering, swappable ranking strategies,
// and user rank highlights outside top tiers.
template <typename T>
class Leaderboard {
private:
    std::vector<T> entries_;

public:
    Leaderboard() = default;

    void addEntry(const T& entry) {
        entries_.push_back(entry);
    }

    void clear() {
        entries_.clear();
    }

    // Filters entries by scope and target identifier
    std::vector<T> filter(LeaderboardScope scope, const std::string& target) const {
        if (scope == LeaderboardScope::GLOBAL || target.empty() || target == "ALL") {
            return entries_;
        }

        std::vector<T> filtered;
        for (const auto& item : entries_) {
            if (scope == LeaderboardScope::BY_COLLEGE && item.college == target) {
                filtered.push_back(item);
            } else if (scope == LeaderboardScope::BY_BRANCH && item.branch == target) {
                filtered.push_back(item);
            } else if (scope == LeaderboardScope::BY_SUBJECT && item.course == target) {
                filtered.push_back(item);
            }
        }
        return filtered;
    }

    // Sorts entries using an injected ranking strategy
    static void sortWithStrategy(std::vector<T>& list, const IRankingStrategy& strategy) {
        std::sort(list.begin(), list.end(), [&strategy](const T& a, const T& b) {
            return strategy.isHigherRank(a, b);
        });
    }

    // Finds 1-based rank of a user
    static int findUserRank(const std::vector<T>& list, const std::string& userId) {
        for (size_t i = 0; i < list.size(); ++i) {
            if (list[i].userId == userId) {
                return static_cast<int>(i + 1);
            }
        }
        return -1;
    }

    // Displays the filtered and ranked leaderboard to console
    static void display(const std::vector<T>& list,
                        const std::string& currentUserId = "",
                        int topN = 10,
                        const std::string& strategyName = "Total Score") {
        std::cout << "\n======================== LEADERBOARD (Ranked by: " << strategyName << ") ========================\n";
        std::cout << std::left 
                  << std::setw(6)  << "Rank"
                  << std::setw(18) << "Name"
                  << std::setw(14) << "College"
                  << std::setw(10) << "Branch"
                  << std::setw(8)  << "Streak"
                  << std::setw(8)  << "Solved"
                  << std::setw(8)  << "Score"
                  << "\n";
        std::cout << "------------------------------------------------------------------------------------\n";

        int rank = 1;
        bool userInTop = false;
        int displayCount = std::min(static_cast<int>(list.size()), topN);

        for (int i = 0; i < displayCount; ++i) {
            const auto& entry = list[i];
            bool isCurrent = (entry.userId == currentUserId);
            if (isCurrent) userInTop = true;

            std::cout << (isCurrent ? "> " : "  ")
                      << std::left
                      << std::setw(4)  << rank++
                      << std::setw(18) << entry.name.substr(0, 16)
                      << std::setw(14) << entry.college.substr(0, 12)
                      << std::setw(10) << entry.branch.substr(0, 8)
                      << std::setw(8)  << (std::to_string(entry.streak) + "d")
                      << std::setw(8)  << entry.challengesSolved
                      << std::setw(8)  << entry.totalScore
                      << "\n";
        }

        // If user is outside top N, print user's exact standing
        if (!currentUserId.empty() && !userInTop) {
            int userRank = findUserRank(list, currentUserId);
            if (userRank > 0 && static_cast<size_t>(userRank - 1) < list.size()) {
                const auto& userEntry = list[userRank - 1];
                std::cout << "------------------------------------------------------------------------------------\n";
                std::cout << "> "
                          << std::left
                          << std::setw(4)  << userRank
                          << std::setw(18) << userEntry.name.substr(0, 16)
                          << std::setw(14) << userEntry.college.substr(0, 12)
                          << std::setw(10) << userEntry.branch.substr(0, 8)
                          << std::setw(8)  << (std::to_string(userEntry.streak) + "d")
                          << std::setw(8)  << userEntry.challengesSolved
                          << std::setw(8)  << userEntry.totalScore
                          << "  (You are #" << userRank << ")\n";
            }
        }

        std::cout << "====================================================================================\n\n";
    }

    const std::vector<T>& getEntries() const noexcept {
        return entries_;
    }

    static std::string renderProgressBar(int current, int total, int barWidth = 10) {
        if (total <= 0) total = 1;
        if (current < 0) current = 0;
        if (current > total) current = total;

        double percentage = (static_cast<double>(current) / total) * 100.0;
        int filled = static_cast<int>((static_cast<double>(current) / total) * barWidth);

        std::ostringstream oss;
        oss << current << "/" << total << " [";
        for (int i = 0; i < barWidth; ++i) {
            if (i < filled) oss << "#";
            else oss << "-";
        }
        oss << "] " << static_cast<int>(percentage) << "%";
        return oss.str();
    }
};
