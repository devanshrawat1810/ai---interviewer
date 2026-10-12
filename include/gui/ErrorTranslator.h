#pragma once

#include <string>
#include <exception>

// OOP Concept: Single Responsibility Principle & Adapter Pattern
// Translates low-level, database, AI, and domain exceptions into actionable, friendly user messages.
class ErrorTranslator {
public:
    static std::string translate(const std::exception& e);
    static std::string translate(const std::string& rawError);
};
