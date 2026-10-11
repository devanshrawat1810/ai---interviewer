#include "core/CodingQuestion.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#endif

namespace fs = std::filesystem;

// OOP Concept: Polymorphism & Subsystem Abstraction
// Implementation of local compilation, execution, and test-case verification.
//
// NOTE FOR PRODUCTION DEPLOYMENT:
// This implementation executes solutions directly on the host OS for local college demonstration.
// For any public-facing or production environment, solutions MUST be containerized/sandboxed
// (e.g. using Docker, nsjail, libseccomp, or WebAssembly) to prevent arbitrary code execution,
// filesystem tampering, and denial-of-service exploits.

CodingQuestion::CodingQuestion(std::string id,
                               std::string subject,
                               std::string topic,
                               Difficulty difficulty,
                               std::string text,
                               std::string explanation,
                               std::string starterCode,
                               std::vector<TestCase> testCases,
                               int points)
    : Question(std::move(id),
               std::move(subject),
               std::move(topic),
               difficulty,
               "coding",
               std::move(text),
               std::move(explanation)),
      starterCode_(std::move(starterCode)),
      testCases_(std::move(testCases)),
      points_(points) {}

void CodingQuestion::display() const {
    std::cout << "\n[" << difficultyToString(difficulty_) << "] Coding Challenge: " << text_ << "\n";
    std::cout << "---------------- Starter Code ----------------\n";
    std::cout << starterCode_ << "\n";
    std::cout << "----------------------------------------------\n";
}

bool CodingQuestion::evaluate(const std::string& userResponse) const {
    ExecutionResult result = runSolution(userResponse);
    return result.compiled && (result.passedCases == result.totalCases) && (result.totalCases > 0);
}

ExecutionResult CodingQuestion::runSolution(const std::string& sourceCode, int timeoutSeconds) const {
    ExecutionResult result;
    result.totalCases = static_cast<int>(testCases_.size());

    if (sourceCode.empty()) {
        result.compiled = false;
        result.compilerOutput = "Error: Submitted code is empty.";
        return result;
    }

    // Determine temporary paths
    std::string tempDir = "temp_exec";
    try {
        fs::create_directories(tempDir);
    } catch (...) {}

    std::string srcFile = tempDir + "/submission.cpp";
    std::string binFile = tempDir + "/submission_bin";
#ifdef _WIN32
    binFile += ".exe";
#endif

    // Write source code to file
    {
        std::ofstream out(srcFile);
        if (!out) {
            result.compiled = false;
            result.compilerOutput = "Failed to write temporary source file.";
            return result;
        }
        out << sourceCode;
    }

    // Build compilation command
    std::string compileLog = tempDir + "/compile.log";
    std::string compileCmd;
#ifdef _WIN32
    // On Windows, prefer g++ if in PATH, otherwise try cl.exe
    compileCmd = "g++ -std=c++17 -O2 \"" + srcFile + "\" -o \"" + binFile + "\" > \"" + compileLog + "\" 2>&1";
#else
    compileCmd = "g++ -std=c++17 -O2 \"" + srcFile + "\" -o \"" + binFile + "\" > \"" + compileLog + "\" 2>&1";
#endif

    int compileExit = std::system(compileCmd.c_str());

    // Read compilation log
    std::ifstream compLog(compileLog);
    std::stringstream logStream;
    if (compLog) {
        logStream << compLog.rdbuf();
    }
    result.compilerOutput = logStream.str();

    if (compileExit != 0 || !fs::exists(binFile)) {
        result.compiled = false;
        if (result.compilerOutput.empty()) {
            result.compilerOutput = "Compilation failed with exit code: " + std::to_string(compileExit);
        }
        return result;
    }

    result.compiled = true;

    // Execute against each test case
    int passed = 0;
    int caseIdx = 1;

    for (const auto& tc : testCases_) {
        std::string inputFile = tempDir + "/input_" + std::to_string(caseIdx) + ".txt";
        std::string outputFile = tempDir + "/output_" + std::to_string(caseIdx) + ".txt";

        {
            std::ofstream inFile(inputFile);
            inFile << tc.input;
        }

        // Run binary with input redirect
        std::string runCmd = "\"" + binFile + "\" < \"" + inputFile + "\" > \"" + outputFile + "\"";
        
        auto start = std::chrono::steady_clock::now();
        int runExit = std::system(runCmd.c_str());
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - start).count();

        if (elapsed > timeoutSeconds) {
            result.caseLogs.push_back("Test Case #" + std::to_string(caseIdx) + ": Time Limit Exceeded (> " +
                                     std::to_string(timeoutSeconds) + "s)");
            ++caseIdx;
            continue;
        }

        if (runExit != 0) {
            result.caseLogs.push_back("Test Case #" + std::to_string(caseIdx) + ": Runtime Error (exit code " +
                                     std::to_string(runExit) + ")");
            ++caseIdx;
            continue;
        }

        // Read output
        std::ifstream outFile(outputFile);
        std::stringstream outStream;
        if (outFile) {
            outStream << outFile.rdbuf();
        }
        std::string actualOutput = outStream.str();

        // Normalize trailing whitespace
        while (!actualOutput.empty() && (actualOutput.back() == '\n' || actualOutput.back() == '\r' || actualOutput.back() == ' ')) {
            actualOutput.pop_back();
        }
        std::string expectedNorm = tc.expectedOutput;
        while (!expectedNorm.empty() && (expectedNorm.back() == '\n' || expectedNorm.back() == '\r' || expectedNorm.back() == ' ')) {
            expectedNorm.pop_back();
        }

        if (actualOutput == expectedNorm) {
            ++passed;
            result.caseLogs.push_back("Test Case #" + std::to_string(caseIdx) + ": PASSED" +
                                     (tc.isHidden ? " (Hidden Case)" : ""));
        } else {
            std::string log = "Test Case #" + std::to_string(caseIdx) + ": FAILED";
            if (!tc.isHidden) {
                log += "\n    Expected: [" + expectedNorm + "]\n    Actual:   [" + actualOutput + "]";
            } else {
                log += " (Hidden Case)";
            }
            result.caseLogs.push_back(log);
        }
        ++caseIdx;
    }

    result.passedCases = passed;
    result.scorePercentage = (result.totalCases > 0)
        ? (static_cast<double>(passed) / result.totalCases) * 100.0
        : 0.0;

    return result;
}

const std::string& CodingQuestion::getStarterCode() const noexcept {
    return starterCode_;
}

const std::vector<TestCase>& CodingQuestion::getTestCases() const noexcept {
    return testCases_;
}

int CodingQuestion::getPoints() const noexcept {
    return points_;
}
