#include "core/Difficulty.h"
#include <algorithm>

// OOP Concept: Encapsulation
// Implementation of Difficulty helper routines.

std::string difficultyToString(Difficulty diff) {
    switch (diff) {
        case Difficulty::EASY:   return "EASY";
        case Difficulty::MEDIUM: return "MEDIUM";
        case Difficulty::HARD:   return "HARD";
    }
    return "EASY";
}

Difficulty difficultyFromString(const std::string& str) {
    std::string upper = str;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    if (upper == "MEDIUM") return Difficulty::MEDIUM;
    if (upper == "HARD")   return Difficulty::HARD;
    return Difficulty::EASY;
}

int difficultyToWeight(Difficulty diff) {
    switch (diff) {
        case Difficulty::EASY:   return 1;
        case Difficulty::MEDIUM: return 2;
        case Difficulty::HARD:   return 3;
    }
    return 1;
}
