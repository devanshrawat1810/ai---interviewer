# AI Interviewer: Demonstration Guide & Presentation Scripts

This guide provides step-by-step instructions for running reliable, compelling project presentations for evaluators.

---

## 1. Demo Credentials & Accounts

The following demo accounts are permanently available after running `scripts/demo_reset.sh` or `scripts/demo_reset.ps1`:

| Role | Email | Password | College / Details | Key Features to Highlight |
| :--- | :--- | :--- | :--- | :--- |
| **Admin** | `admin@college.edu` | `Admin@123` | Computer Science Dept | System metrics, question authoring, validation rules, soft deletes |
| **Student 1** | `alice@college.edu` | `Admin@123` | MIT College of Engg | 7-day active streak, 520 pts, unlocked badges, recent interview history |
| **Student 2** | `bob@college.edu` | `Admin@123` | Stanford Tech | 3-day active streak, 280 pts, ECE track, bookmarks |
| **Student 3** | `charlie@college.edu` | `Admin@123` | IIT Bombay | 1-day active streak, 140 pts, fresh account for live mock interview demo |

---

## 2. Setting Up 100% Offline Demo Mode

To guarantee that the demo works with **zero internet connection** and **no API key**:

### On Linux / macOS:
```bash
bash scripts/demo_reset.sh
export DEMO_MODE=1
./build/ai_interviewer
```

### On Windows (PowerShell):
```powershell
.\scripts\demo_reset.ps1
$env:DEMO_MODE="1"
.\build\Release\ai_interviewer.exe
```

When `DEMO_MODE=1` is set:
- A `[*** DEMO MODE ACTIVE - Offline LLM Emulation ***]` badge is displayed in the candidate header.
- AI feedback and analysis use pre-configured, realistic qualitative commentary.

---

## 3. Ten-Minute Live Presentation Script

### Minute 0–2: Problem Statement & Architecture
- **Script**: *"Hello everyone. Traditional campus placement preparation tools rely either on static multiple-choice tests or opaque cloud services. We built the AI Interviewer: a modern C++17 object-oriented platform featuring progressive difficulty interview sessions, daily streaks, automated coding challenges, and a provider-neutral AI layer with automatic offline fallback."*
- **Action**: Run the demo reset script to show clean initialization.

### Minute 2–4: Student Login & Gamification
- **Script**: *"Let us log in as Alice Johnson (alice@college.edu). Notice the greeting banner showing her 7-day streak, highest streak record, and unlocked achievement badges."*
- **Action**: Display the monthly ASCII calendar grid showing active practice days.

### Minute 4–7: Live Mock Interview Session
- **Script**: *"Now let us start a mock interview. Notice the progressive ramp: Questions start at Easy, transition to Medium, and escalate to Hard based on candidate performance. Each question features strict countdown timers."*
- **Action**: Answer 2–3 questions. Show immediate keyword/MCQ evaluation. Complete the session.
- **Script**: *"Upon completion, the system generates a dual evaluation: deterministic numerical scoring combined with multidimensional AI feedback analyzing clarity, structure, and next steps."*

### Minute 7–9: Admin Governance Panel
- **Script**: *"Now let us switch to the Administrator account (admin@college.edu). The service layer strictly enforces Role-Based Access Control (RBAC)."*
- **Action**: Open Admin Panel. Demonstrate question validation (e.g. attempting to add an MCQ with fewer than 2 options triggers a validation exception). Show soft deletion keeping historical interview integrity intact.

### Minute 9–10: Q&A & Viva Defense
- **Script**: *"The architecture strictly follows SOLID principles and the Dependency Inversion Principle, allowing seamless switching between local JSON storage and MongoDB."*
- **Action**: Refer to [`docs/OOP_MAP.md`](file:///C:/Users/Deepu/.gemini/antigravity/scratch/ai-interviewer/docs/OOP_MAP.md) for evaluator questions.

---

## 4. Three-Minute Quick Elevator Demo

1. **Step 1 (0:00 - 0:45)**: Run `scripts/demo_reset.ps1`, launch with `DEMO_MODE=1`, log in as `alice@college.edu`. Highlight streaks and badges.
2. **Step 2 (0:45 - 2:00)**: Start a practice interview, answer questions, display instant evaluation, and show the comprehensive feedback report.
3. **Step 3 (2:00 - 3:00)**: Log in as `admin@college.edu`, show telemetry metrics and question authoring validation. Conclude with OOP design architecture.

---

## 5. Failover / Backup Plan

- **If MongoDB is stopped**: The app automatically detects this via `ping()` and falls back to JSON files without crashing.
- **If LLM API is down or key expires**: Set `DEMO_MODE=1` or unset `AI_API_KEY` to demonstrate graceful fallback to deterministic rule-based evaluation.
- **If terminal displays distorted characters**: The app uses standard ASCII borders and safe UTF-8 glyphs compatible with standard terminals.
