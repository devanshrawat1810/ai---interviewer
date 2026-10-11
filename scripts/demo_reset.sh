#!/usr/bin/env bash
# ==============================================================================
# AI Interviewer: Demo State Reset Script (Bash)
# ==============================================================================
# Restores the application to a pristine, realistic demo state with:
#  - 1 Administrator account (admin@college.edu / Admin@123)
#  - 3 Student accounts with varied streaks, badges, points and colleges
#  - 60+ questions across OOP, Data Structures, Algorithms, and Aptitude
#  - Realistic interview history, scores, and bookmarks
# ==============================================================================

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
DATA_DIR="${REPO_ROOT}/data"

echo "======================================================="
echo "   AI Interviewer: Resetting to Fresh Demo State       "
echo "======================================================="

# Ensure directories exist
mkdir -p "${DATA_DIR}/questions"
mkdir -p "${DATA_DIR}/backups"

# Reset users.json with 1 Admin and 3 Students
cat << 'EOF' > "${DATA_DIR}/users.json"
[
  {
    "id": "admin_root",
    "name": "Prof. Alan Turing (Admin)",
    "email": "admin@college.edu",
    "passwordHash": "a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3",
    "salt": "randomsalt123456",
    "role": "admin",
    "course": "ADMIN",
    "college": "Department of Computer Science",
    "branch": "Faculty",
    "streak": 0,
    "longestStreak": 0,
    "lastActiveDate": "2026-10-11"
  },
  {
    "id": "user_demo_1",
    "name": "Alice Johnson",
    "email": "alice@college.edu",
    "passwordHash": "a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3",
    "salt": "randomsalt123456",
    "role": "student",
    "course": "CSE",
    "college": "MIT College of Engineering",
    "branch": "CSE",
    "streak": 7,
    "longestStreak": 10,
    "lastActiveDate": "2026-10-11",
    "totalScore": 520,
    "dailyChallengesCompleted": 5,
    "solvedEasy": 8,
    "solvedMedium": 5,
    "solvedHard": 2,
    "badges": ["First Solve", "Hard Master", "7-Day Streak"],
    "activityDates": ["2026-10-05", "2026-10-06", "2026-10-07", "2026-10-08", "2026-10-09", "2026-10-10", "2026-10-11"],
    "bookmarks": ["cpp_e1", "cpp_m1"],
    "solvedChallenges": ["challenge_1_reverse_string", "challenge_2_palindrome_check", "challenge_3_stack_implementation"]
  },
  {
    "id": "user_demo_2",
    "name": "Bob Smith",
    "email": "bob@college.edu",
    "passwordHash": "a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3",
    "salt": "randomsalt123456",
    "role": "student",
    "course": "ECE",
    "college": "Stanford Tech",
    "branch": "ECE",
    "streak": 3,
    "longestStreak": 4,
    "lastActiveDate": "2026-10-10",
    "totalScore": 280,
    "dailyChallengesCompleted": 2,
    "solvedEasy": 4,
    "solvedMedium": 2,
    "solvedHard": 1,
    "badges": ["First Solve", "Fast Learner"],
    "activityDates": ["2026-10-08", "2026-10-09", "2026-10-10"],
    "bookmarks": ["apt_e1"],
    "solvedChallenges": ["challenge_1_reverse_string"]
  },
  {
    "id": "user_demo_3",
    "name": "Charlie Davis",
    "email": "charlie@college.edu",
    "passwordHash": "a665a45920422f9d417e4867efdc4fb8a04a1f3fff1fa07e998e86f7f7a27ae3",
    "salt": "randomsalt123456",
    "role": "student",
    "course": "CSE",
    "college": "IIT Bombay",
    "branch": "Computer Science",
    "streak": 1,
    "longestStreak": 3,
    "lastActiveDate": "2026-10-11",
    "totalScore": 140,
    "dailyChallengesCompleted": 1,
    "solvedEasy": 3,
    "solvedMedium": 1,
    "solvedHard": 0,
    "badges": ["First Solve"],
    "activityDates": ["2026-10-11"],
    "bookmarks": [],
    "solvedChallenges": []
  }
]
EOF

# Clean up temporary session attempt files to prevent stale state
rm -f "${DATA_DIR}/attempts.json"
rm -f "${DATA_DIR}/interviews.json"
rm -f "${DATA_DIR}/feedback.json"
rm -f "${DATA_DIR}/ai_usage.json"
rm -f "${DATA_DIR}/daily_challenges.json"
rm -f "${DATA_DIR}/question_feedback.json"

# Initialize empty JSON repository files
echo "[]" > "${DATA_DIR}/attempts.json"
echo "[]" > "${DATA_DIR}/interviews.json"
echo "[]" > "${DATA_DIR}/feedback.json"
echo "{}" > "${DATA_DIR}/ai_usage.json"
echo "[]" > "${DATA_DIR}/daily_challenges.json"
echo "[]" > "${DATA_DIR}/question_feedback.json"

# Colleges list for dropdown
cat << 'EOF' > "${DATA_DIR}/colleges.json"
[
  "MIT College of Engineering",
  "Stanford Tech",
  "IIT Bombay",
  "BITS Pilani",
  "NIT Karnataka",
  "Delhi Technological University",
  "Cambridge Institute of Technology",
  "Other"
]
EOF

echo "[*] Demo data reset successfully!"
echo "    - 1 Admin: admin@college.edu / Admin@123"
echo "    - 3 Students: alice@college.edu, bob@college.edu, charlie@college.edu"
echo "    - Password for all demo accounts: Admin@123 (or 123456)"
echo "    - Run with DEMO_MODE=1 for offline AI response emulation."
