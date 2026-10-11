cmake_minimum_required(VERSION 3.16)
project(ai_interviewer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Compiler warnings flags
if(MSVC)
    add_compile_options(/W4 /permissive-)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find nlohmann_json
find_package(nlohmann_json CONFIG QUIET)
if(NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
    )
    FetchContent_MakeAvailable(json)
endif()

# Find GTest
find_package(GTest CONFIG QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

# Find MongoDB C++ driver (mongocxx and bsoncxx)
find_package(bsoncxx CONFIG QUIET)
find_package(mongocxx CONFIG QUIET)

if(mongocxx_FOUND AND bsoncxx_FOUND)
    message(STATUS "MongoDB C++ Driver (mongocxx/bsoncxx) found!")
    add_compile_definitions(AI_INTERVIEWER_HAS_MONGODB=1)
    if(TARGET mongo::mongocxx_shared)
        set(MONGO_CXX_TARGETS mongo::mongocxx_shared mongo::bsoncxx_shared)
    elseif(TARGET mongo::mongocxx_static)
        set(MONGO_CXX_TARGETS mongo::mongocxx_static mongo::bsoncxx_static)
    endif()
else()
    message(WARNING "MongoDB C++ Driver not found. Building with stub/fallback mode for mongocxx headers.")
endif()

# Core library source files
set(CORE_SOURCES
    src/core/Difficulty.cpp
    src/core/Question.cpp
    src/core/MCQQuestion.cpp
    src/core/TextQuestion.cpp
    src/core/CodingQuestion.cpp
    src/core/User.cpp
    src/core/Student.cpp
    src/core/Admin.cpp
    src/core/Badge.cpp
    src/core/BadgeRule.cpp
    src/core/SystemClock.cpp
    src/core/FakeClock.cpp
    src/auth/Sha256.cpp
    src/auth/AuthService.cpp
    src/admin/AdminService.cpp
    src/course/Topic.cpp
    src/course/Subject.cpp
    src/course/Course.cpp
    src/interview/LinearRampStrategy.cpp
    src/interview/AdaptiveStrategy.cpp
    src/interview/InterviewSession.cpp
    src/interview/SessionFactory.cpp
    src/interview/DailyChallengeService.cpp
    src/interview/StreakService.cpp
    src/interview/ReviewService.cpp
    src/interview/NotificationService.cpp
    src/feedback/FeedbackGenerator.cpp
    src/feedback/RuleBasedFeedback.cpp
    src/feedback/AIFeedback.cpp
    src/db/Database.cpp
    src/db/UserMapper.cpp
    src/db/QuestionMapper.cpp
    src/db/AttemptMapper.cpp
    src/db/InterviewMapper.cpp
    src/db/FeedbackMapper.cpp
    src/db/ScoreboardMapper.cpp
    src/db/JsonUserRepository.cpp
    src/db/JsonQuestionRepository.cpp
    src/db/JsonAttemptRepository.cpp
    src/db/JsonInterviewRepository.cpp
    src/db/JsonFeedbackRepository.cpp
    src/db/JsonScoreboardRepository.cpp
    src/db/JsonDailyChallengeRepository.cpp
    src/db/JsonQuestionFeedbackRepository.cpp
    src/db/MongoUserRepository.cpp
    src/db/MongoQuestionRepository.cpp
    src/db/MongoAttemptRepository.cpp
    src/db/MongoInterviewRepository.cpp
    src/db/MongoFeedbackRepository.cpp
    src/db/MongoScoreboardRepository.cpp
    src/db/MongoDailyChallengeRepository.cpp
    src/db/MongoQuestionFeedbackRepository.cpp
    src/db/RepositoryFactory.cpp
    src/ui/RankingStrategy.cpp
    src/ui/ConsoleUI.cpp
)

add_library(ai_interviewer_core STATIC ${CORE_SOURCES})
target_link_libraries(ai_interviewer_core PUBLIC nlohmann_json::nlohmann_json ${MONGO_CXX_TARGETS})

# Main executable
add_executable(ai_interviewer src/main.cpp)
target_link_libraries(ai_interviewer PRIVATE ai_interviewer_core)

# Migration and Seed Tool executable
add_executable(seed_db tools/seed_db.cpp)
target_link_libraries(seed_db PRIVATE ai_interviewer_core)

# Unit & Integration Tests
enable_testing()
add_executable(ai_interviewer_tests
    tests/test_auth.cpp
    tests/test_questions.cpp
    tests/test_coding_execution.cpp
    tests/test_strategies.cpp
    tests/test_interview.cpp
    tests/test_progression_badges.cpp
    tests/test_mappers.cpp
    tests/test_factory_fallback.cpp
    tests/test_repository_contract.cpp
    tests/test_admin.cpp
    tests/test_daily_challenge.cpp
    tests/test_streak.cpp
)
target_link_libraries(ai_interviewer_tests PRIVATE
    ai_interviewer_core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ai_interviewer_tests)
