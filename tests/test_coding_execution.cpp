#include <gtest/gtest.h>
#include "core/CodingQuestion.h"
#include <filesystem>

namespace fs = std::filesystem;

class CodingExecutionTest : public ::testing::Test {
protected:
    void TearDown() override {
        try {
            if (fs::exists("temp_exec")) {
                fs::remove_all("temp_exec");
            }
        } catch (...) {}
    }
};

TEST_F(CodingExecutionTest, CompileErrorCapturedCleanly) {
    std::vector<TestCase> testCases = {
        {"5\n", "10", false}
    };

    CodingQuestion q(
        "cq_test_1", "cpp_oop", "syntax", Difficulty::EASY,
        "Double the number", "Multiply by 2",
        "int main() {}", testCases, 50
    );

    // Code with syntax error
    std::string invalidCode =
        "#include <iostream>\n"
        "int main() {\n"
        "    syntax_error_undefined_identifier;\n"
        "    return 0;\n"
        "}\n";

    ExecutionResult result = q.runSolution(invalidCode, 2);
    EXPECT_FALSE(result.compiled);
    EXPECT_FALSE(result.compilerOutput.empty());
    EXPECT_EQ(result.passedCases, 0);
}

TEST_F(CodingExecutionTest, ValidSolutionCompilesAndRuns) {
    std::vector<TestCase> testCases = {
        {"4\n", "8", false},
        {"10\n", "20", false}
    };

    CodingQuestion q(
        "cq_test_2", "cpp_oop", "math", Difficulty::EASY,
        "Double the number", "Multiply by 2",
        "int main() {}", testCases, 50
    );

    std::string validCode =
        "#include <iostream>\n"
        "int main() {\n"
        "    int x;\n"
        "    if (std::cin >> x) {\n"
        "        std::cout << (x * 2) << std::endl;\n"
        "    }\n"
        "    return 0;\n"
        "}\n";

    ExecutionResult result = q.runSolution(validCode, 2);
    // If g++ is available, it compiles and passes
    if (result.compiled) {
        EXPECT_EQ(result.passedCases, 2);
        EXPECT_DOUBLE_EQ(result.scorePercentage, 100.0);
    } else {
        // If g++ is not installed on this test machine, compilation was attempted
        EXPECT_FALSE(result.compilerOutput.empty());
    }
}

TEST_F(CodingExecutionTest, EmptySubmissionRejected) {
    CodingQuestion q(
        "cq_test_3", "cpp_oop", "math", Difficulty::EASY,
        "Test", "Explanation",
        "", {}, 50
    );

    ExecutionResult result = q.runSolution("", 2);
    EXPECT_FALSE(result.compiled);
    EXPECT_EQ(result.passedCases, 0);
}
