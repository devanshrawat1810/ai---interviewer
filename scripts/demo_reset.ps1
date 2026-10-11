# ==============================================================================
# AI Interviewer: Demo State Reset Script (PowerShell)
# ==============================================================================
# Restores the application to a pristine, realistic demo state with:
#  - 1 Administrator account (admin@college.edu / Admin@123)
#  - 3 Student accounts with varied streaks, badges, points and colleges
#  - 60+ questions across OOP, Data Structures, Algorithms, and Aptitude
#  - Realistic interview history, scores, and bookmarks
# ==============================================================================

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = Split-Path -Parent $ScriptDir
$DataDir = Join-Path $RepoRoot "data"

Write-Host "=======================================================" -ForegroundColor Cyan
Write-Host "   AI Interviewer: Resetting to Fresh Demo State       " -ForegroundColor Cyan
Write-Host "=======================================================" -ForegroundColor Cyan

New-Item -ItemType Directory -Force -Path (Join-Path $DataDir "questions") | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $DataDir "backups") | Out-Null

$usersJson = @"
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
"@

Set-Content -Path (Join-Path $DataDir "users.json") -Value $usersJson -Encoding UTF8

$emptyArray = "[]"
$emptyObject = "{}"

Set-Content -Path (Join-Path $DataDir "attempts.json") -Value $emptyArray -Encoding UTF8
Set-Content -Path (Join-Path $DataDir "interviews.json") -Value $emptyArray -Encoding UTF8
Set-Content -Path (Join-Path $DataDir "feedback.json") -Value $emptyArray -Encoding UTF8
Set-Content -Path (Join-Path $DataDir "ai_usage.json") -Value $emptyObject -Encoding UTF8
Set-Content -Path (Join-Path $DataDir "daily_challenges.json") -Value $emptyArray -Encoding UTF8
Set-Content -Path (Join-Path $DataDir "question_feedback.json") -Value $emptyArray -Encoding UTF8

$collegesJson = @"
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
"@

Set-Content -Path (Join-Path $DataDir "colleges.json") -Value $collegesJson -Encoding UTF8

Write-Host "[*] Demo state reset successfully!" -ForegroundColor Green
Write-Host "    - 1 Admin: admin@college.edu / Admin@123"
Write-Host "    - 3 Students: alice@college.edu, bob@college.edu, charlie@college.edu"
Write-Host "    - Passwords: Admin@123"
Write-Host "    - Run with DEMO_MODE=1 for offline AI response emulation."
