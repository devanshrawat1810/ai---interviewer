# AI Interviewer: C++17 Placement Simulation & Coding Platform

[![CI Quality Pipeline](https://github.com/example/ai-interviewer/actions/workflows/ci.yml/badge.svg)](https://github.com/example/ai-interviewer/actions)
[![Release Packaging](https://github.com/example/ai-interviewer/actions/workflows/release.yml/badge.svg)](https://github.com/example/ai-interviewer/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/Standard-C%2B%2B17-orange.svg)](CMakeLists.txt)
[![Tests: 20 Suites](https://img.shields.io/badge/Google%20Test-20%20Suites-brightgreen.svg)](tests/)

An Object-Oriented C++17 mock interview simulation and coding practice platform designed for college placement preparation. Features progressive difficulty interview sessions, gamified daily streaks and badges, automated coding challenge evaluations with timeouts, an administrative governance panel, an interchangeable repository layer supporting **MongoDB** and **JSON file persistence**, and a provider-neutral AI layer with 100% offline fallback.

> **Academic & Viva Reference**:
> - [The Viva Sheet (OOP Concepts & Design Patterns)](docs/OOP_MAP.md)
> - [Comprehensive Viva Q&A (40 Technical Questions)](docs/VIVA_QA.md)
> - [System Architecture & Layering Specs](docs/ARCHITECTURE.md)
> - [UML Class & Sequence Diagrams](docs/UML.md)
> - [Live Demonstration & Presentation Guide](docs/DEMO.md)
> - [Testing Strategy & Test Double Matrix](docs/TESTING.md)
> - [Security Model & Sandboxing Notes](docs/SECURITY.md)
> - [Formal College Project Report Skeleton](docs/REPORT.md)
> - [12-Slide Final Presentation Outline](docs/PRESENTATION.md)


---

## 1. Quick Start

### Prerequisites
- **CMake** 3.16+
- **C++17 Compiler** (GCC 9+, Clang 10+, or MSVC 2019+)
- (Optional) **Docker & Docker Compose** for local MongoDB
- (Optional) **vcpkg** for dependency manifest management

### Clone & Build
```bash
# Clone the repository
git clone https://github.com/example/ai-interviewer.git
cd ai-interviewer

# Configure build with CMake
# (If using vcpkg manifest mode)
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake

# (If building without vcpkg, CMake automatically fetches GTest and nlohmann_json via FetchContent)
cmake -B build -S .

# Build all targets (Main app, Seed tool, Unit tests)
cmake --build build --config Release
```

### Run
```bash
# Run the Console application
./build/ai_interviewer_console         # Linux/macOS
.\build\Release\ai_interviewer_console.exe # Windows MSVC
```

### Option C: Running the Qt 6 Desktop GUI (`ai_interviewer_gui`)
The project features a modern native Qt 6 desktop frontend implemented via Model-View-Presenter (MVP).
- **Prerequisite**: Qt 6.5+ (Widgets, Concurrent, Test). (Do NOT install via vcpkg; use the official Qt Online Installer or distro package manager `qt6-base-dev`).
- **Configure & Build GUI**:
```bash
# Windows MinGW / Ninja Example:
cmake -B build_gui -S . -G "Ninja" -DBUILD_GUI=ON -DCMAKE_PREFIX_PATH="C:/Qt/6.11.2/mingw_64"
cmake --build build_gui --target ai_interviewer_gui ai_interviewer_gui_tests

# Run Qt GUI Tests (Headless):
$env:QT_QPA_PLATFORM="offscreen"
.\build_gui\ai_interviewer_gui_tests.exe

# Launch GUI Desktop App:
.\build_gui\ai_interviewer_gui.exe
```
> **Deployment (Windows)**: Run `windeployqt.exe .\build_gui\ai_interviewer_gui.exe` to package all required Qt DLLs and plugins for standalone distribution.

---

## 2. Storage Backends: JSON vs. MongoDB

The system strictly adheres to the **Dependency Inversion Principle**. Business services, interview engines, and the UI depend exclusively on abstract repository interfaces (`IUserRepository`, `IQuestionRepository`, `IAttemptRepository`, `IDailyChallengeRepository`, `IQuestionFeedbackRepository`, etc.).

### Option A: Running with JSON Storage (Default)
Out of the box, `STORAGE_BACKEND=json` is active. No external database setup is required. All state is persisted to human-readable JSON files in `data/`:
```bash
export STORAGE_BACKEND=json     # Linux/macOS
set STORAGE_BACKEND=json        # Windows CMD
$env:STORAGE_BACKEND="json"     # Windows PowerShell
./ai_interviewer
```

### Option B: Running with MongoDB (Dual Backend)
1. **Start MongoDB via Docker Compose**:
   ```bash
   docker compose up -d
   ```
2. **Configure environment variables**:
   ```bash
   export STORAGE_BACKEND=mongo
   export MONGODB_URI=mongodb://localhost:27017
   export MONGODB_DB=ai_interviewer
   ```
3. **Run the Database Seed Tool** to initialize questions, test users, and collections:
   ```bash
   ./build/seed_db
   ```
4. **Launch Application**:
   ```bash
   ./build/ai_interviewer
   ```
> **Automatic Fallback Guarantee**: If `STORAGE_BACKEND=mongo` is configured but the MongoDB server is unreachable or fails the ping health check, `RepositoryFactory` logs a warning and automatically falls back to local JSON repositories without crashing.

---

## 3. Seeded Accounts & Credentials

For immediate evaluation, the following pre-configured accounts are provided in `data/users.json` and MongoDB seeds:

| Role | Email | Password | College | Branch | Purpose |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Admin** | `admin@college.edu` | `Admin@123` | NIT Karnataka | CSE | System administration, question authoring & analytics |
| **Student** | `student_cse@college.edu` | `Pass@123` | IIT Bombay | Computer Science | Active streaks, interview progression & challenge history |
| **Student** | `student_ece@college.edu` | `Pass@123` | BITS Pilani | Electronics | Branch-specific leaderboard & review telemetry |

*(New users can also be registered directly through the Console UI with college and branch fields.)*

---

## 4. Phase 3 Feature Suite

### 1. College & Branch Profiles
- Registration collects `name`, `email`, `password`, `course`, `college`, and `branch`.
- Profile editing allows students to update their college or branch at any time.
- Fully backward compatible: existing users without these fields automatically default to `"Unspecified"`.

### 2. Admin Governance Panel
- **Role-Based Access Control (RBAC)**: All administrative methods in `AdminService` verify `caller.isAdmin()` at the service layer, throwing an exception if unauthorized.
- **Question Authoring & Validation**: Rigorous checks via `ValidationException` (MCQ requires $\ge 2$ options and valid answer; Text questions require keywords; valid difficulty).
- **Soft Deletion**: Questions use an `isActive` flag so that historical interview session records and question attempts never refer to missing items.
- **Bulk JSON Import**: Import questions from external JSON files with real-time per-line progress reporting (accepted vs. rejected counts with explicit failure reasons).
- **System Telemetry & Reporting**: Live metrics for total registered users, users active today, attempt distribution by subject, and difficulty discrepancy flags.

### 3. Deterministic Daily Challenge
- `DailyChallengeService` selects exactly one challenge per course per calendar day deterministically using date-seeded hashing.
- Every student enrolled in the same course encounters the same daily problem on the same date.
- **Anti-Repetition Algorithm**: Ensures no question repeats within a 30-day window.
- **Rewards**: Solving the daily challenge awards **+50 bonus points** and advances the student's daily streak.
- Injected `IClock` abstraction (`SystemClock` / `FakeClock`) for testability across date boundaries.

### 4. Streak Tracking & ASCII Calendar
- `StreakService` monitors daily student activity dates (`YYYY-MM-DD`).
- **Streak Logic**: Consecutive calendar days increment the streak and update the `longestStreak`. Skipping a day automatically resets the active streak to 1 upon the next activity.
- **At-Risk Warning**: Identifies if a user has not logged an activity today and warns them in the notification banner.
- **ASCII Calendar Grid**: Renders an intuitive monthly calendar visualization in the terminal showing days practiced (`[X]`) versus idle days (`[ ]`).

### 5. Multi-Scope Filtered Leaderboard & Ranking Strategies
- **Strategy Pattern (`RankingStrategy`)**: Toggle leaderboard rankings on demand:
  1. *Total Score Strategy*: Ranks by aggregated challenge and interview points.
  2. *Problems Solved Strategy*: Ranks by total correct problem completions.
  3. *Active Streak Strategy*: Ranks by current daily activity streak.
- **Multi-Scope Scopes**: Filter standings **Globally**, **By College**, **By Branch**, or **By Enrolled Subject**.
- **Candidate Highlighting**: Displays the top 10 students, and if the viewing candidate is ranked below #10, appends a dedicated row highlighting their exact position.

### 6. Review & Practice Tools
- **Retry Incorrect Attempts**: Fetches past wrong answers for any subject, allowing students to retry without modifying original historical records.
- **Question Bookmarking**: Students can bookmark challenging questions during interviews or reviews and revisit them from a dedicated bookmarks menu.
- **Question Issue Reporting**: Students can submit reports (flagging unclear wording or difficulty discrepancies), which flow into the Admin panel for review.

### 7. Notification Service & Login Banner
- Generates a greeting upon login that displays:
  - Current streak and longest streak record.
  - Streak preservation alert if the streak is at risk of resetting today.
  - Daily challenge status (available vs. completed).
  - Unlocked achievement badges.

---

---

## 5. Phase 4: AI Interviewer Suite

Phase 4 introduces provider-neutral, LLM-powered feedback, dynamic follow-up questioning, resume-based mock interviews, and personalized answer coaching with full offline/graceful fallback.

### 1. Provider-Neutral Architecture
The LLM abstraction uses the **Strategy** and **Factory** patterns:
- **`ILLMClient`**: Interface declaring `complete(const LLMRequest&) -> LLMResponse`.
- **`OpenAICompatibleClient`**: Targets OpenAI API models (`gpt-4o`, `gpt-4o-mini`) and Gemini's OpenAI-compatible endpoint.
- **`AnthropicClient`**: Targets Anthropic Claude API (`claude-3-5-sonnet-20241022`, `claude-3-haiku-20240307`) using the Claude Messages API format (`anthropic-version: 2023-06-01`).
- **`MockLLMClient`**: Test double supporting scripted responses and error simulation without real network calls.
- **`NullLLMClient`**: Safe Null Object returned when no API key is configured. Throws `AiUnavailableException` to seamlessly trigger offline rule-based fallbacks.

### 2. Supported Features
- **AI-Powered Feedback (`AIFeedback`)**: Extends `FeedbackGenerator`. Combines deterministic scoring with qualitative LLM evaluations (communication clarity 1-5, answer structure 1-5, confidence cues 1-5, strengths, weaknesses, and concrete recommendations).
- **Dynamic Follow-Up Questions (`FollowUpGenerator`)**: Triggers after a student provides a substantive answer on Text/HR questions, generating a tailored follow-up question that digs deeper into what the student actually said.
- **Resume-Based Mock Interviews (`ResumeInterviewMode` & `ResumeAnalyzer`)**: Extracts skills, projects, and domain experience from a student's resume (max 6,000 characters) and generates 8-10 customized interview questions mapped to existing question models.
- **Answer Review Coaching (`AnswerReviewer`)**: On-demand single-answer analysis providing a score (1-5), specific strengths, improvement areas, and a rewritten model answer.
- **Usage & Rate Limiting (`UsageTracker` & `ResponseCache`)**: Enforces daily AI quotas (default 30 calls/day, 5 answer reviews/day) and caches responses across identical prompts.

### 3. AI Setup & Environment Variables

Configure the following variables in your terminal or `.env` file:

```bash
# Provider selection: 'openai', 'gemini', 'anthropic', or 'mock'
export AI_PROVIDER=openai

# Your API Key (leave unset or empty to use offline rule-based mode)
export AI_API_KEY="your-api-key-here"

# Model selection
export AI_MODEL="gpt-4o-mini"

# Optional Base URL override (e.g. for local Ollama, vLLM, or reverse proxies)
export AI_BASE_URL="https://api.openai.com/v1"

# Request timeout in seconds (default: 30)
export AI_TIMEOUT_SECONDS=30

# Daily call quota per user (default: 30)
export AI_DAILY_LIMIT=30
```

#### Obtaining an API Key
- **OpenAI**: Sign up at [platform.openai.com](https://platform.openai.com/), navigate to API Keys, and create a new secret key.
- **Gemini**: Sign up at [aistudio.google.com](https://aistudio.google.com/), generate an API key, set `AI_PROVIDER=gemini`, `AI_MODEL=gemini-1.5-flash`, and optionally `AI_BASE_URL=https://generativelanguage.googleapis.com/v1beta/openai/`.
- **Anthropic**: Sign up at [console.anthropic.com](https://console.anthropic.com/), navigate to API Keys, set `AI_PROVIDER=anthropic`, and set `AI_MODEL=claude-3-5-sonnet-20241022`.

### 4. Privacy & Security Guarantees
- **Prompt Injection Defense**: All user inputs (student answers, resume text) are sanitized via `PromptLibrary::sanitizeInput()` to neutralize jailbreak delimiters, role-override tags (`[SYSTEM]`), and recursive instructions.
- **Untrusted Data Delimiters**: Prompts wrap user content within explicit `<<<STUDENT_ANSWER>>>` and `<<<RESUME_CONTENT>>>` markers with strict system instructions prohibiting directive execution.
- **Resume Data Privacy**: Students must explicitly provide consent before resume text is processed. Raw resumes are **never** stored in the database or logged; only student-confirmed extracted skill tags are persisted to the profile.
- **API Key Protection**: API keys are never stored, logged, or printed. All debug strings mask keys using `HttpClient::maskApiKey()` (e.g., `sk-****1234`).

---

## 6. Manual Testing Checklist (for Real API Key Verification)

When testing live LLM connectivity with an actual API key:

- [ ] **Offline Status**: Run app with `AI_API_KEY=""`. Ensure dashboard displays `AI Features: Disabled` and mock interviews generate rule-based feedback without errors.
- [ ] **OpenAI Connectivity**: Set `AI_PROVIDER=openai`, `AI_API_KEY=sk-...`, and complete an interview. Verify "AI-generated feedback" appears with clarity ratings (1-5).
- [ ] **Gemini Connectivity**: Set `AI_PROVIDER=gemini` and verify compatible completion endpoint handling.
- [ ] **Anthropic Connectivity**: Set `AI_PROVIDER=anthropic` and verify Messages API v1 request handling.
- [ ] **Resume Upload & Consent**: Select option 3 in the student menu, paste a resume under 6,000 characters, confirm consent, and verify that tailored questions reflect your resume projects.
- [ ] **Resume Size Limit**: Attempt pasting a resume with $>6,000$ characters and confirm validation rejection.
- [ ] **Daily Quota Enforcement**: Set `AI_DAILY_LIMIT=2`, execute 2 AI interviews, and confirm the third triggers a quota limit message.
- [ ] **Answer Review Coaching**: Select option 8, input an answer, and verify that a 1-5 rating, critiques, and an improved model answer are returned.

---

## 7. OOP Concepts & Design Patterns Reference

| Concept / Pattern | Component | Demonstration |
| :--- | :--- | :--- |
| **Abstraction** | `Question`, `QuestionSelectionStrategy`, `FeedbackGenerator`, `RankingStrategy`, `IClock`, `ILLMClient`, `IHttpTransport`, `IAiUsageRepository` | Pure virtual interfaces defining strict contracts without implementation details. |
| **Inheritance & Polymorphism** | `MCQQuestion`, `TextQuestion`, `CodingQuestion`, `Student`, `Admin`, `AIFeedback`, `RuleBasedFeedback`, `OpenAICompatibleClient`, `AnthropicClient`, `MockLLMClient`, `NullLLMClient` | Derived classes overriding virtual methods; runtime polymorphism via base class pointers. |
| **Encapsulation** | `User`, `Student`, `InterviewSession`, `AdminService`, `HttpClient`, `UsageTracker` | Hidden state with protected accessors, cryptographic salting/hashing, and immutable IDs. |
| **Composition** | `Course` $\rightarrow$ `Subject` $\rightarrow$ `Topic`, `AIFeedback` (has `ILLMClient` and fallback `RuleBasedFeedback`) | Curricula hierarchy and feedback fallback mechanisms modeled through object containment. |
| **Strategy Pattern** | `LinearRampStrategy`, `AdaptiveStrategy`, `RankingStrategy`, `BadgeRule`, `ILLMClient` implementations | Interchangeable algorithms for question ramps, leaderboard ranking, badge rules, and LLM providers. |
| **Factory Pattern** | `SessionFactory`, `RepositoryFactory`, `LLMClientFactory` | Centralized creation and wiring of polymorphic sessions, repository bundles, and AI clients. |
| **Null Object Pattern** | `NullLLMClient` | Safe no-op object preventing null pointer dereferences and enabling seamless offline fallback. |
| **Dependency Inversion (DIP)** | Repositories & Services (`IUserRepository`, `IClock`, `IAiUsageRepository`, `IHttpTransport`) | High-level business logic depends on interfaces, decoupling code from external services. |
| **Singleton Pattern** | `Database` | Single thread-safe management point for Mongo driver instance and connection pools. |
| **RAII Pattern** | `HttpClient` | Resource Acquisition Is Initialization for managing curl global/easy handles safely. |

---

## 8. Running Tests

The test suite consists of **20 Google Test specifications** covering unit logic, domain rules, algorithms, typed repository contracts, and Phase 4 AI modules (all mocked, zero real network calls):

```bash
# Run tests using CTest
ctest --test-dir build --output-on-failure

# Or execute the test binary directly
./build/ai_interviewer_tests          # Linux/macOS
.\build\Release\ai_interviewer_tests.exe  # Windows MSVC
```

### Complete Test Suites:
1. `test_auth.cpp`: Registration, salting, SHA-256 hashing, login attempt throttling.
2. `test_questions.cpp`: MCQ and Text keyword evaluation, difficulty scaling.
3. `test_coding_questions.cpp`: Coding challenge subprocess execution and timeout protection.
4. `test_interview.cpp`: Adaptive ramping and interview session record tracking.
5. `test_feedback.cpp`: Rule-based interview performance reports and scoring.
6. `test_progression_badges.cpp`: Gamification tier unlocking and badge triggers.
7. `test_mappers.cpp`: JSON and BSON object mapper round-tripping for all fields.
8. `test_repository_contract.cpp`: Google Test Type-Parameterized tests verifying JSON and Mongo repos.
9. `test_admin.cpp`: Admin RBAC enforcement, validation exceptions, soft deletes, bulk imports.
10. `test_daily_challenge.cpp`: Deterministic daily seed selection, 30-day anti-repeat, FakeClock time travel.
11. `test_streak.cpp`: Consecutive streaks, missed day resets, longest streak, ASCII calendar grid.
12. `test_ranking_strategy.cpp`: Score, Solved, and Streak ranking strategies with custom candidate data.
13. `test_http_client.cpp`: FakeTransport retry logic, backoff, 429/5xx retries, 4xx abort, API key masking.
14. `test_llm_clients.cpp`: LLMClientFactory provider selection, NullLLMClient offline mode, MockLLMClient scripting.
15. `test_response_parser.cpp`: Markdown fence stripping, JSON schema extraction, score clamping, repair fallbacks.
16. `test_ai_feedback.cpp`: AIFeedback generation, parse failure fallback, network error fallback.
17. `test_followup.cpp`: Follow-up generation, max-3 limits, half-points score awarding.
18. `test_prompt_injection.cpp`: PromptLibrary sanitization, jailbreak neutralization, input size truncation.
19. `test_resume_mode.cpp`: Resume length validation (>6,000 characters rejection), privacy consent, LLM skill extraction.
20. `test_usage_tracker.cpp`: Daily call quota countdown, calendar date rollover, answer review rate limits.

