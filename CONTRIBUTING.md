# 🤝 Contributing to BDIX Server Monitor

First off, thank you for considering contributing to BDIX Server Monitor! It's people like you that make this tool better for everyone.

## 📋 Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [How to Contribute](#how-to-contribute)
4. [Style Guide](#style-guide)
5. [Submission Guidelines](#submission-guidelines)

## 📜 Code of Conduct

This project and everyone participating in it is governed by a Code of Conduct. By participating, you are expected to uphold this code. Please report unacceptable behavior to [email](mailto:faisalmoshiur+bdixftp@gmail.com).

## 🚀 Getting Started

1. **Fork the repository** on GitHub.
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/fam007e/bdix-server-monitor.git
   ```
3. **Install dependencies** (see README.md).
4. **Create a branch** for your feature or bugfix:
   ```bash
   git checkout -b feature/amazing-feature
   ```

## 🛠️ How to Contribute

### Reporting Bugs

- Ensure the bug was not already reported by searching on GitHub under [Issues](https://github.com/fam007e/bdix-server-monitor/issues).
- If you're unable to find an open issue addressing the problem, open a new one. Be sure to include a **title and clear description**, as much relevant information as possible, and a **code sample** or an **executable test case** demonstrating the expected behavior that is not occurring.

### Suggesting Enhancements

- Open a new issue with the label `enhancement`.
- Clearly explain the specific behavior you would like to see.
- Explain why this enhancement would be useful to most users.

### Pull Requests

1. Fill in the required template.
2. Do not include issue numbers in the PR title.
3. Include screenshots and animated GIFs in your pull request whenever possible.
4. Follow the [Style Guide](#style-guide).
5. Document new code based on the [Documentation Styleguide](#documentation-styleguide).
6. End all files with a newline.

## 🎨 Style Guide

### C Coding Standards

- We follow **C17** standards.
- Use **snake_case** for variable and function names.
- Use **PascalCase** for structs and enums.
- Indent using **4 spaces**.
- Use `//` for comments.
- Always check return values of memory allocation and system calls.
- Use the `defer` macro for resource cleanup where appropriate.

Example:
```c
int calculate_latency(const char *url) {
    if (!url) return -1;
    // Implementation...
}
```

### Git Commit Messages

- Use the present tense ("Add feature" not "Added feature").
- Use the imperative mood ("Move cursor to..." not "Moves cursor to...").
- Limit the first line to 72 characters or less.
- Reference issues and pull requests liberally after the first line.

## 📝 Submission Guidelines

1. **Run Tests**: Ensure all tests pass before submitting.
   ```bash
   make check
   ```
2. **Format Code**: Run the formatter.
   ```bash
   make format
   ```
3. **Static Analysis**: Check for warnings.
   ```bash
   make analyze
   ```

## ❓ Questions

If you have questions, feel free to start a discussion at [GitHub Discussions](https://github.com/fam007e/bdix-server-monitor/discussions).
