# AI Interviewer: Verification & Testing Strategy

This document outlines the testing architecture, validation methodology, and test execution procedures.

---

## 1. Testing Architecture & Test Double Strategy

The testing suite adheres strictly to modern C++ quality standards:
- **Zero Real Network Calls**: Tests for HTTP and LLM integrations utilize `FakeTransport` and `MockLLMClient`, isolating unit tests from network latency, rate limits, and third-party outages.
- **Deterministic Time Simulation**: Gamification services (`StreakService`, `DailyChallengeService`) accept an injected `IClock` interface, allowing `FakeClock` to simulate calendar boundaries and leap years deterministically.
- **Type-Parameterized Repository Contract Tests**: Google Test's `TYPED_TEST_SUITE` verifies that `JsonUserRepository` and `MongoUserRepository` satisfy identical behavioral contracts.

---

## 2. Test Execution Commands

```bash
# 1. Run all tests via CTest
ctest --test-dir build --output-on-failure

# 2. Run specific test suites directly
./build/ai_interviewer_tests --gtest_filter=HttpClientTest.*
./build/ai_interviewer_tests --gtest_filter=AIFeedbackTest.*
./build/ai_interviewer_tests --gtest_filter=DailyChallengeTest.*
./build/ai_interviewer_tests --gtest_filter=StreakTest.*

# 3. Run Architectural Layer Boundary Verification
python scripts/check_layers.py
python -m unittest tests/test_check_layers.py
```

---

## 3. Test Suites Overview (20 Total Suites)

| Suite Name | Source File | Key Verification Objectives |
| :--- | :--- | :--- |
| **Auth** | `test_auth.cpp` | SHA-256 password salting, hashing, authentication, and lockout limits. |
| **Questions** | `test_questions.cpp` | MCQ answer evaluation, Text keyword matching, and point scaling. |
| **Coding** | `test_coding_execution.cpp` | Subprocess compilation, test case verification, and timeout termination. |
| **Strategies** | `test_strategies.cpp` | Linear ramp and adaptive difficulty scaling algorithms. |
| **Interview** | `test_interview.cpp` | Session state transitions, timer limits, and response recordings. |
| **Badges** | `test_progression_badges.cpp` | Gamification tier unlocking, streak milestones, and score criteria. |
| **Mappers** | `test_mappers.cpp` | Lossless round-tripping between C++ objects, JSON, and BSON. |
| **Repo Contract** | `test_repository_contract.cpp` | Typed verification of JSON vs MongoDB CRUD operations. |
| **Admin** | `test_admin.cpp` | Role-Based Access Control, question validation rules, and bulk import. |
| **Daily Challenge**| `test_daily_challenge.cpp` | Deterministic date-seeded problem generation and 30-day anti-repeat filter. |
| **Streak** | `test_streak.cpp` | Consecutive day increments, missed-day resets, and monthly ASCII grid. |
| **Ranking** | `test_ranking_strategy.cpp` | Leaderboard sorting by Score, Solved Count, and Streak. |
| **HTTP Client** | `test_http_client.cpp` | Exponential backoff, retry on 429/5xx, no retry on 4xx, key masking. |
| **LLM Clients** | `test_llm_clients.cpp` | Client factory resolution, `NullLLMClient` offline mode, `MockLLMClient`. |
| **Response Parser**| `test_response_parser.cpp` | Code fence stripping, JSON schema extraction, and rating clamping. |
| **AI Feedback** | `test_ai_feedback.cpp` | Multidimensional feedback generation, JSON error fallback, and network fallback. |
| **Follow-Up** | `test_followup.cpp` | Dynamic follow-up generation, max-3 per interview limit, half points. |
| **Injection** | `test_prompt_injection.cpp` | Input sanitization, jailbreak neutralization, and score integrity preservation. |
| **Resume Mode** | `test_resume_mode.cpp` | 6,000-character size rejection, privacy consent, and skill extraction. |
| **Usage Tracker**| `test_usage_tracker.cpp` | Daily AI call limits, date rollover resets, and answer review caps. |
