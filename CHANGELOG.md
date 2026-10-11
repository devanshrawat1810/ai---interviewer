# Changelog

All notable changes to the **AI Interviewer** project will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2026-10-11

### Added
- **Core Domain & Engine (Phase 1)**:
  - Runtime polymorphic question hierarchy (`Question` $\rightarrow$ `MCQQuestion`, `TextQuestion`, `CodingQuestion`).
  - Interactive 10-question mock interviews with countdown timers.
  - Linear ramp and adaptive difficulty scaling strategies.
  - Salted SHA-256 user authentication with student and admin role models.
  - Automated C++ coding challenge execution with subprocess timeout management.
- **MongoDB Integration (Phase 2)**:
  - Singleton `Database` managing single `mongocxx::instance` lifecycle.
  - Dual-backend repository architecture implementing identical typed contracts.
  - Automatic MongoDB failure detection with seamless fallback to local JSON files.
- **Gamification & Administration (Phase 3)**:
  - Consecutive streak tracking and monthly ASCII practice calendar grid.
  - Deterministic date-seeded Daily Challenges (+50 bonus points) with 30-day anti-repetition filter.
  - Multi-metric Leaderboard (Score, Solved Count, Streak) with global, college, and branch scopes.
  - Admin governance panel with question authoring validation and soft deletion.
  - Question bookmarking and student issue reporting workflows.
- **Provider-Neutral AI Suite (Phase 4)**:
  - `ILLMClient` abstraction supporting OpenAI, Google Gemini, and Anthropic Claude.
  - `HttpClient` RAII curl wrapper with exponential backoff retries and key masking.
  - `AIFeedback` combining deterministic technical scoring with multidimensional qualitative metrics.
  - Dynamic follow-up question generation for substantive text answers.
  - Resume-based mock interview mode with size limits, privacy consent, and zero raw resume storage.
  - Daily AI call quotas (`UsageTracker`) and in-memory prompt response caching (`ResponseCache`).
  - `NullLLMClient` providing 100% offline rule-based fallback without an API key.
- **Desktop GUI Architecture (Phase 5)**:
  - Qt 6 Widgets front-end design using Model-View-Presenter (MVP).
  - Clean separation of UI views from business logic with constructor-injected `ServiceContext`.
- **Quality Audit, CI & Academic Suite (Phase 6)**:
  - GitHub Actions multi-platform CI with MongoDB container and AddressSanitizer (ASan).
  - Architectural layering boundary enforcement script (`scripts/check_layers.py`).
  - Self-contained demo state reset scripts (`scripts/demo_reset.sh`, `scripts/demo_reset.ps1`).
  - 100% offline demonstration mode via `DEMO_MODE=1`.
  - Comprehensive documentation suite: `OOP_MAP.md`, `VIVA_QA.md`, `ARCHITECTURE.md`, `UML.md`, `DEMO.md`, `TESTING.md`, `SECURITY.md`, `REPORT.md`, `PRESENTATION.md`.
