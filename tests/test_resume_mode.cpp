#include <gtest/gtest.h>
#include "ai/ResumeAnalyzer.h"
#include "interview/ResumeInterviewMode.h"
#include "ai/MockLLMClient.h"
#include <string>

TEST(ResumeModeTest, SizeLimitRejectionExceeding6000Chars) {
    std::string oversized(6500, 'A');
    std::string errorMsg;
    bool valid = ResumeInterviewMode::validateResumeLength(oversized, errorMsg);
    EXPECT_FALSE(valid);
    EXPECT_NE(errorMsg.find("6000"), std::string::npos);
}

TEST(ResumeModeTest, ValidLengthAccepted) {
    std::string validText = "Software Engineer with 2 years of experience in C++ and Python.";
    std::string errorMsg;
    bool valid = ResumeInterviewMode::validateResumeLength(validText, errorMsg);
    EXPECT_TRUE(valid);
    EXPECT_TRUE(errorMsg.empty());
}

TEST(ResumeModeTest, ConsentNoticeContainsPrivacyAssurances) {
    std::string notice = ResumeInterviewMode::getPrivacyConsentNotice();
    EXPECT_NE(notice.find("consent"), std::string::npos);
    EXPECT_NE(notice.find("raw resume"), std::string::npos);
}

TEST(ResumeModeTest, AnalyzeResumeExtractsSkillsAndGeneratesQuestions) {
    std::string mockResponse = R"({
        "skills": ["C++", "STL", "Design Patterns"],
        "projects": ["Distributed Cache", "Game Engine"],
        "experience_summary": "Junior Developer with systems programming focus",
        "questions": [
            {
                "id": "res_q1",
                "topic": "C++",
                "difficulty": "MEDIUM",
                "question": "How did you manage memory in your Game Engine?",
                "explanation": "Smart pointers and RAII",
                "keywords": ["smart pointer", "unique_ptr", "raii", "leak"]
            },
            {
                "id": "res_q2",
                "topic": "Distributed Cache",
                "difficulty": "HARD",
                "question": "How did you handle cache invalidation in Distributed Cache?",
                "explanation": "TTL and LRU eviction policy",
                "keywords": ["ttl", "lru", "eviction", "invalidation"]
            }
        ]
    })";

    auto mock = std::make_shared<MockLLMClient>(mockResponse);
    ResumeAnalyzer analyzer(mock);

    std::string resume = "C++ developer. Built a Distributed Cache and a Game Engine using modern C++.";
    auto result = analyzer.analyzeResume(resume, "Core C++");

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.extractedSkills.size(), 3u);
    EXPECT_EQ(result.extractedProjects.size(), 2u);
    EXPECT_EQ(result.experienceSummary, "Junior Developer with systems programming focus");
    EXPECT_EQ(result.generatedQuestions.size(), 2u);
    EXPECT_EQ(result.generatedQuestions[0]->getTopic(), "C++");
}

TEST(ResumeModeTest, FallbackQuestionsGeneratedOnLLMFailure) {
    auto mock = std::make_shared<MockLLMClient>("invalid json");
    ResumeAnalyzer analyzer(mock);

    std::string resume = "Developer skilled in OOP and C++.";
    auto result = analyzer.analyzeResume(resume, "OOP Track");

    EXPECT_TRUE(result.success); // fallback kicks in
    EXPECT_GE(result.generatedQuestions.size(), 5u);
    EXPECT_EQ(result.experienceSummary, "Candidate with background in OOP Track");
}
