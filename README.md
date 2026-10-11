# AI Interviewer (C++17 Console Application)

An Object-Oriented C++17 mock interview simulation and coding practice platform designed for college placement preparation. Features progressive difficulty interview sessions, gamified daily streaks and badges, automated coding challenge evaluations with timeouts, an administrative governance panel, and an interchangeable repository layer supporting **MongoDB** and **JSON file persistence**.

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
# Run the application
./build/ai_interviewer         # Linux/macOS
.\build\Release\ai_interviewer.exe # Windows MSVC
```

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

## 5. OOP Concepts & Design Patterns Reference

| Concept / Pattern | Component | Demonstration |
| :--- | :--- | :--- |
| **Abstraction** | `Question`, `QuestionSelectionStrategy`, `FeedbackGenerator`, `RankingStrategy`, `IClock` | Pure virtual interfaces defining strict contracts without implementation details. |
| **Inheritance & Polymorphism** | `MCQQuestion`, `TextQuestion`, `CodingQuestion`, `Student`, `Admin`, `SystemClock`, `FakeClock` | Derived classes overriding virtual methods; runtime polymorphism via base class pointers. |
| **Encapsulation** | `User`, `Student`, `InterviewSession`, `AdminService` | Hidden state with protected accessors, cryptographic salting/hashing, and immutable IDs. |
| **Composition** | `Course` $\rightarrow$ `Subject` $\rightarrow$ `Topic` | Curricula hierarchy modeled through strict composition (lifecycle containment). |
| **Strategy Pattern** | `LinearRampStrategy`, `AdaptiveStrategy`, `RankingStrategy`, `BadgeRule` | Interchangeable algorithms for question ramps, leaderboard ranking, and badge rules. |
| **Factory Pattern** | `SessionFactory`, `RepositoryFactory` | Centralized creation and wiring of polymorphic sessions and repository bundles. |
| **Dependency Inversion (DIP)** | Repositories & Services (`IUserRepository`, `IClock`, etc.) | High-level business logic depends on interfaces, decoupling code from Mongo/JSON storage. |
| **Singleton Pattern** | `Database` | Single thread-safe management point for Mongo driver instance and connection pools. |
| **Template Programming** | `Leaderboard<T>` | Generic ranking engine compatible with diverse candidate containers. |

---

## 6. Running Tests

The test suite consists of **12 Google Test specifications** covering unit logic, domain rules, algorithms, and typed repository contracts:

```bash
# Run tests using CTest
ctest --test-dir build --output-on-failure

# Or execute the test binary directly
./build/ai_interviewer_tests          # Linux/macOS
.\build\Release\ai_interviewer_tests.exe  # Windows MSVC
```

### Test Suites Included:
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
