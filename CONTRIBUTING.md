# Contributing to AI Interviewer

Thank you for your interest in contributing to **AI Interviewer**!

---

## 1. Code of Conduct & Standards
- **C++ Standard**: C++17 only.
- **Memory Safety**: No raw `new` or `delete`. Use `std::unique_ptr` and `std::shared_ptr`.
- **Compiler Warnings**: Code must compile under `-Wall -Wextra` with **zero warnings**.
- **Object-Oriented Integrity**: Every new class must include a brief comment identifying which OOP concept or design pattern it demonstrates.
- **Architectural Layering**: Never violate layer boundaries. UI code must not query DB classes directly; domain code must remain decoupled from storage and presentation. Run `python scripts/check_layers.py` before submitting a pull request.

---

## 2. Pull Request Process
1. Fork the repository and create a descriptive feature branch (`git checkout -b feat/your-feature`).
2. Implement your changes adhering to existing design patterns.
3. Add unit tests in `tests/` covering new functionality.
4. Verify that all 20 Google Test suites pass (`ctest --test-dir build --output-on-failure`).
5. Run the architectural layer checker:
   ```bash
   python scripts/check_layers.py
   ```
6. Commit changes with clear, semantic commit messages and open a Pull Request.
