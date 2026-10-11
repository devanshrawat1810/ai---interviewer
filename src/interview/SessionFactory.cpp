#include "interview/SessionFactory.h"
#include "interview/LinearRampStrategy.h"
#include "interview/AdaptiveStrategy.h"
#include <chrono>

// OOP Concept: Factory Method Pattern
// Concrete factory implementation creating customized session variants.

std::unique_ptr<InterviewSession> SessionFactory::createSession(
    SessionType type,
    const std::string& userId,
    const std::string& subjectId,
    const std::vector<std::shared_ptr<Question>>& questionPool) {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    std::string sessionId = "sess_" + std::to_string(millis);

    switch (type) {
        case SessionType::PRACTICE: {
            // Practice: Adaptive strategy, rapid 5 questions
            auto strategy = std::make_unique<AdaptiveStrategy>(Difficulty::EASY);
            return std::make_unique<InterviewSession>(
                sessionId, userId, subjectId, "practice", std::move(strategy), questionPool, 5);
        }
        case SessionType::MOCK: {
            // Mock Interview: Linear ramp (Easy -> Med -> Hard), full 10 questions
            auto strategy = std::make_unique<LinearRampStrategy>();
            return std::make_unique<InterviewSession>(
                sessionId, userId, subjectId, "mock", std::move(strategy), questionPool, 10);
        }
        case SessionType::DAILY: {
            // Daily Challenge: Linear ramp, 5 questions
            auto strategy = std::make_unique<LinearRampStrategy>();
            return std::make_unique<InterviewSession>(
                sessionId, userId, subjectId, "daily", std::move(strategy), questionPool, 5);
        }
    }

    auto defaultStrategy = std::make_unique<LinearRampStrategy>();
    return std::make_unique<InterviewSession>(
        sessionId, userId, subjectId, "mock", std::move(defaultStrategy), questionPool, 10);
}
