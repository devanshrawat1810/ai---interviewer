#pragma once

#include <memory>
#include <string>
#include <vector>
#include "interview/InterviewSession.h"
#include "core/Question.h"

// Types of interview sessions supported
enum class SessionType {
    PRACTICE,
    MOCK,
    DAILY
};

// OOP Concept: Factory Method Pattern
// Encapsulates creation of various interview session variants with the appropriate
// strategy, question count, and timer policies.
class SessionFactory {
public:
    static std::unique_ptr<InterviewSession> createSession(
        SessionType type,
        const std::string& userId,
        const std::string& subjectId,
        const std::vector<std::shared_ptr<Question>>& questionPool);
};
