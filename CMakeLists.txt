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
# Find libcurl
find_package(CURL QUIET)
if(CURL_FOUND)
    message(STATUS "libcurl found!")
    add_compile_definitions(AI_INTERVIEWER_HAS_CURL=1)
    set(CURL_TARGETS CURL::libcurl)
else()
    message(WARNING "libcurl not found via CMake find_package. HttpClient will use mock/offline mode unless linked.")
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
    src/interview/ResumeInterviewMode.cpp
    src/feedback/FeedbackGenerator.cpp
    src/feedback/RuleBasedFeedback.cpp
    src/feedback/AIFeedback.cpp
    src/ai/HttpClient.cpp
    src/ai/AnthropicClient.cpp
    src/ai/OpenAICompatibleClient.cpp
    src/ai/MockLLMClient.cpp
    src/ai/NullLLMClient.cpp
    src/ai/LLMClientFactory.cpp
    src/ai/PromptLibrary.cpp
    src/ai/ResponseParser.cpp
    src/ai/FollowUpGenerator.cpp
    src/ai/ResumeAnalyzer.cpp
    src/ai/AnswerReviewer.cpp
    src/ai/UsageTracker.cpp
    src/ai/ResponseCache.cpp
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
    src/db/JsonAiUsageRepository.cpp
    src/db/MongoUserRepository.cpp
    src/db/MongoQuestionRepository.cpp
    src/db/MongoAttemptRepository.cpp
    src/db/MongoInterviewRepository.cpp
    src/db/MongoFeedbackRepository.cpp
    src/db/MongoScoreboardRepository.cpp
    src/db/MongoDailyChallengeRepository.cpp
    src/db/MongoQuestionFeedbackRepository.cpp
    src/db/MongoAiUsageRepository.cpp
    src/db/RepositoryFactory.cpp
    src/ui/RankingStrategy.cpp
    src/ui/ConsoleUI.cpp
)

add_library(ai_interviewer_core STATIC ${CORE_SOURCES})
target_link_libraries(ai_interviewer_core PUBLIC nlohmann_json::nlohmann_json ${MONGO_CXX_TARGETS} ${CURL_TARGETS})

# Console Application executable
add_executable(ai_interviewer_console src/main.cpp)
target_link_libraries(ai_interviewer_console PRIVATE ai_interviewer_core)

# Compatibility alias for earlier scripts
add_executable(ai_interviewer src/main.cpp)
target_link_libraries(ai_interviewer PRIVATE ai_interviewer_core)

# Migration and Seed Tool executable
add_executable(seed_db tools/seed_db.cpp)
target_link_libraries(seed_db PRIVATE ai_interviewer_core)

# Option to build Qt 6 Desktop GUI
option(BUILD_GUI "Build Qt 6 Desktop GUI front-end" ON)

if(BUILD_GUI)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC OFF)
    set(CMAKE_AUTORCC ON)

    find_package(Qt6 COMPONENTS Widgets Concurrent Test QUIET)

    if(Qt6_FOUND)
        message(STATUS "Qt 6 found! Configuring ai_interviewer_gui and gui tests.")

        set(GUI_HEADERS
            include/gui/Result.h
            include/gui/ThemeTokens.h
            include/gui/ThemeManager.h
            include/gui/ErrorTranslator.h
            include/gui/ServiceContext.h
            include/gui/Navigator.h
            include/gui/widgets/PrimaryButton.h
            include/gui/widgets/Card.h
            include/gui/widgets/FormField.h
            include/gui/widgets/PasswordField.h
            include/gui/widgets/DifficultyBadge.h
            include/gui/widgets/ProgressRing.h
            include/gui/widgets/ChartWidget.h
            include/gui/widgets/CppSyntaxHighlighter.h
            include/gui/widgets/QuestionViewFactory.h
            include/gui/widgets/MCQQuestionWidget.h
            include/gui/widgets/TextQuestionWidget.h
            include/gui/widgets/CodingQuestionWidget.h
            include/gui/presenters/LoginPresenter.h
            include/gui/presenters/DashboardPresenter.h
            include/gui/presenters/CourseSelectPresenter.h
            include/gui/presenters/InterviewPresenter.h
            include/gui/views/LoginView.h
            include/gui/views/DashboardView.h
            include/gui/views/CourseSelectView.h
            include/gui/views/InterviewView.h
            include/gui/views/FeedbackView.h
            include/gui/views/ProfileView.h
            include/gui/views/LeaderboardView.h
            include/gui/views/AdminView.h
            include/gui/views/ResumeView.h
            include/gui/views/MainWindow.h
        )

        set(GUI_SOURCES
            ${GUI_HEADERS}
            src/gui/ErrorTranslator.cpp
            src/gui/ThemeManager.cpp
            src/gui/ServiceContext.cpp
            src/gui/Navigator.cpp
            src/gui/widgets/PrimaryButton.cpp
            src/gui/widgets/Card.cpp
            src/gui/widgets/FormField.cpp
            src/gui/widgets/PasswordField.cpp
            src/gui/widgets/DifficultyBadge.cpp
            src/gui/widgets/ProgressRing.cpp
            src/gui/widgets/ChartWidget.cpp
            src/gui/widgets/CppSyntaxHighlighter.cpp
            src/gui/widgets/MCQQuestionWidget.cpp
            src/gui/widgets/TextQuestionWidget.cpp
            src/gui/widgets/CodingQuestionWidget.cpp
            src/gui/widgets/QuestionViewFactory.cpp
            src/gui/presenters/LoginPresenter.cpp
            src/gui/presenters/DashboardPresenter.cpp
            src/gui/presenters/CourseSelectPresenter.cpp
            src/gui/presenters/InterviewPresenter.cpp
            src/gui/views/LoginView.cpp
            src/gui/views/DashboardView.cpp
            src/gui/views/CourseSelectView.cpp
            src/gui/views/InterviewView.cpp
            src/gui/views/FeedbackView.cpp
            src/gui/views/ProfileView.cpp
            src/gui/views/LeaderboardView.cpp
            src/gui/views/AdminView.cpp
            src/gui/views/ResumeView.cpp
            src/gui/views/MainWindow.cpp
        )

        add_executable(ai_interviewer_gui src/gui/main_gui.cpp ${GUI_SOURCES})
        target_link_libraries(ai_interviewer_gui PRIVATE
            ai_interviewer_core
            Qt6::Widgets
            Qt6::Concurrent
        )

        # Qt Test executable
        add_executable(ai_interviewer_gui_tests
            tests/test_gui_presenters.cpp
            ${GUI_SOURCES}
        )
        target_link_libraries(ai_interviewer_gui_tests PRIVATE
            ai_interviewer_core
            Qt6::Widgets
            Qt6::Concurrent
            Qt6::Test
        )
        add_test(NAME ai_interviewer_gui_tests COMMAND ai_interviewer_gui_tests)
    else()
        message(WARNING "Qt 6 not found via find_package. Set CMAKE_PREFIX_PATH to Qt installation to build GUI.")
    endif()
endif()

# Unit & Integration Tests (Google Test)
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
    tests/test_http_client.cpp
    tests/test_llm_clients.cpp
    tests/test_response_parser.cpp
    tests/test_ai_feedback.cpp
    tests/test_followup.cpp
    tests/test_prompt_injection.cpp
    tests/test_resume_mode.cpp
    tests/test_usage_tracker.cpp
)
target_link_libraries(ai_interviewer_tests PRIVATE
    ai_interviewer_core
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(ai_interviewer_tests)


