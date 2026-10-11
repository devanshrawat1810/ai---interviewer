#!/usr/bin/env python3
"""
Architecture Layering Rules Checker
-----------------------------------
Verifies the architectural boundary rules:
1. UI / GUI layer (include/ui, src/ui, include/gui, src/gui) must NOT include
   internal db implementations directly (e.g. Json*Repository.h, Mongo*Repository.h, Database.h).
   It should only access storage through abstract interfaces or RepositoryFactory/ServiceContext.
2. Services (include/interview, src/interview, include/auth, src/auth,
   include/admin, src/admin, include/feedback, src/feedback) must NOT include
   UI or GUI headers.
3. Domain / Core (include/core, src/core, include/course, src/course) must NOT
   include DB, Services, or UI headers.
"""

import sys
import os
import re

ROOT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))

violations = []

def check_file(filepath, forbidden_patterns, rule_name):
    rel_path = os.path.relpath(filepath, ROOT_DIR).replace("\\", "/")
    try:
        with open(filepath, "r", encoding="utf-8", errors="ignore") as f:
            for line_no, line in enumerate(f, 1):
                clean_line = line.strip()
                if not clean_line.startswith("#include"):
                    continue
                for pat in forbidden_patterns:
                    if re.search(pat, clean_line):
                        violations.append(
                            f"[{rule_name}] {rel_path}:{line_no} violates rule with: {clean_line}"
                        )
    except Exception as e:
        print(f"Error reading {filepath}: {e}", file=sys.stderr)

def scan():
    # Rule 1: UI / GUI cannot include concrete DB implementations
    ui_dirs = ["include/ui", "src/ui", "include/gui", "src/gui"]
    forbidden_in_ui = [
        r'["<]db/Json.*Repository\.h[">]',
        r'["<]db/Mongo.*Repository\.h[">]',
        r'["<]db/Database\.h[">]'
    ]

    # Rule 2: Services cannot include UI/GUI headers
    service_dirs = [
        "include/auth", "src/auth",
        "include/admin", "src/admin",
        "include/interview", "src/interview",
        "include/feedback", "src/feedback"
    ]
    forbidden_in_services = [
        r'["<]ui/.*\.h[">]',
        r'["<]gui/.*\.h[">]'
    ]

    # Rule 3: Core / Domain cannot include DB, Services, or UI
    core_dirs = [
        "include/core", "src/core",
        "include/course", "src/course"
    ]
    forbidden_in_core = [
        r'["<]db/.*\.h[">]',
        r'["<]ui/.*\.h[">]',
        r'["<]gui/.*\.h[">]',
        r'["<]interview/.*\.h[">]',
        r'["<]auth/.*\.h[">]',
        r'["<]admin/.*\.h[">]',
        r'["<]feedback/.*\.h[">]'
    ]

    for d in ui_dirs:
        full_d = os.path.join(ROOT_DIR, d)
        if os.path.exists(full_d):
            for root, _, files in os.walk(full_d):
                for f in files:
                    if f.endswith((".h", ".hpp", ".cpp")):
                        check_file(os.path.join(root, f), forbidden_in_ui, "Rule 1 (UI -> DB Internals)")

    for d in service_dirs:
        full_d = os.path.join(ROOT_DIR, d)
        if os.path.exists(full_d):
            for root, _, files in os.walk(full_d):
                for f in files:
                    if f.endswith((".h", ".hpp", ".cpp")):
                        check_file(os.path.join(root, f), forbidden_in_services, "Rule 2 (Services -> UI)")

    for d in core_dirs:
        full_d = os.path.join(ROOT_DIR, d)
        if os.path.exists(full_d):
            for root, _, files in os.walk(full_d):
                for f in files:
                    if f.endswith((".h", ".hpp", ".cpp")):
                        check_file(os.path.join(root, f), forbidden_in_core, "Rule 3 (Core -> DB/Services/UI)")

if __name__ == "__main__":
    scan()
    if violations:
        print(f"FAILED: Found {len(violations)} architectural layer violation(s):\n")
        for v in violations:
            print(f"  * {v}")
        sys.exit(1)
    else:
        print("SUCCESS: Architecture layering rules verified. Zero violations found.")
        sys.exit(0)
