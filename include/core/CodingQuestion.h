#pragma once

#include "core/Question.h"
#include <string>
#include <vector>

// Structure representing a single unit test case for code execution
struct TestCase {
    std::string input;
    std::string expectedOutput;
    bool isHidden{false};
};

// Result summary of compiling and executing a student solution
struct ExecutionResult {
    bool compiled{false};
    std::string compilerOutput;
    int totalCases{0};
    int passedCases{0};
    double scorePercentage{0.0};
    std::vector<std::string> caseLogs;
};

// OOP Concept: Inheritance & Polymorphism
// Specializes Question for live C++ code challenges.
// Evaluates submissions by writing to disk, invoking the compiler, and executing against test cases.
//
// NOTE: This implementation executes code locally for developmental and college demo purposes.
// In any production or public multi-user deployment, execution MUST be securely sandboxed
// using containers (e.g. Docker, nsjail, or gVisor) with resource quotas and restricted system calls.
class CodingQuestion : public Question {
private:
    std::string starterCode_;
    std::vector<TestCase> testCases_;
    int points_;

public:
    CodingQuestion(std::string id,
                   std::string subject,
                   std::string topic,
                   Difficulty difficulty,
                   std::string text,
                   std::string explanation,
                   std::string starterCode,
                   std::vector<TestCase> testCases,
                   int points = 100);

    ~CodingQuestion() override = default;

    void display() const override;
    bool evaluate(const std::string& userResponse) const override;

    // Rich execution interface
    ExecutionResult runSolution(const std::string& sourceCode, int timeoutSeconds = 2) const;

    const std::string& getStarterCode() const noexcept;
    const std::vector<TestCase>& getTestCases() const noexcept;
    int getPoints() const noexcept;
};
