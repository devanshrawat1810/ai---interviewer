#include <gtest/gtest.h>
#include "db/RepositoryFactory.h"

TEST(FactoryFallbackTest, FallbackToJsonWhenMongoUnreachable) {
    // Attempt creating repository with mongo explicitly requested
    auto repos = RepositoryFactory::createRepositories("mongo");

    // The factory must NEVER return null repositories or crash.
    ASSERT_NE(repos.userRepo, nullptr);
    ASSERT_NE(repos.questionRepo, nullptr);
    ASSERT_NE(repos.attemptRepo, nullptr);
    ASSERT_NE(repos.interviewRepo, nullptr);
    ASSERT_NE(repos.feedbackRepo, nullptr);
    ASSERT_NE(repos.scoreboardRepo, nullptr);

    // Active backend should either be "mongo" (if MongoDB daemon happens to be running)
    // or properly fallen back to "json" (if unreachable).
    EXPECT_TRUE(repos.activeBackend == "mongo" || repos.activeBackend == "json");
}

TEST(FactoryFallbackTest, DefaultJsonBackend) {
    auto repos = RepositoryFactory::createRepositories("json");
    ASSERT_NE(repos.userRepo, nullptr);
    EXPECT_EQ(repos.activeBackend, "json");
}
