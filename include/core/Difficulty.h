#pragma once

#include <string>

// OOP Concept: Encapsulation & Type Safety
// Enum class encapsulating question and challenge difficulty levels.
enum class Difficulty {
    EASY,
    MEDIUM,
    HARD
};

std::string difficultyToString(Difficulty diff);
Difficulty difficultyFromString(const std::string& str);
int difficultyToWeight(Difficulty diff);
