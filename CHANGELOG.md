# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Initial setup for `CHANGELOG.md`, `ROADMAP.md`, `FAQ.md`, `SUPPORT.md`.
- GitHub Issue Templates (`bug_report.md`, `feature_request.md`, `config.yml`).
- GitHub Pull Request Template (`PULL_REQUEST_TEMPLATE.md`).
- GitHub Workflows (`discord-webhook.yml`, `issue-slash-cmd.yml`, `release.yml`).

### Changed

### Fixed

### Removed

## [1.0.0] - 2025-12-15

### Added
- Core BDIX Server Monitoring functionality.
- Multithreaded server checking with `libcurl`.
- JSON configuration loading with `jansson`.
- Terminal UI with color-coded output.
- `Makefile` for building, testing, and formatting.
- Initial documentation: `README.md`, `docs/API.md`, `docs/ARCHITECTURE.md`.
- CMake build configuration (`CMakeLists.txt`).
- Unit tests for `server`, `checker`, and `config` modules.
- **Markdown Export**: New feature to save check results to `.md` files.
- `CODE_OF_CONDUCT.md`, `CONTRIBUTING.md`, `SECURITY.md`.

### Changed
- Refactored `defer` macro usage in C source files (`src/checker.c`, `src/config.c`, `src/main.c`) to manual cleanup for standard C compatibility and correctness.
- Increased `MAX_SERVERS_PER_CATEGORY` to 2000 to support larger server lists.
- Updated `README.md` and `CONTRIBUTING.md` with correct GitHub repository links and contact information for Faisal Ahmed Moshiur and email.
- Updated `CODE_OF_CONDUCT.md` and `SECURITY.md` with correct contact email.

### Fixed
- Corrected `common.h` to include necessary POSIX feature macros and `<math.h>` for proper compilation.
- Fixed race condition in `thread_pool.c` causing lost wakeups.
- Fixed garbled UI output by implementing atomic line printing.
- Fixed malformed URLs in `data/server.json`.

### Removed
- Removed placeholder GitHub links (e.g., `yourusername`).
- Removed `docs/CONTRIBUTING.md` as `CONTRIBUTING.md` exists at the root.

## [0.1.0] - 2025-XX-XX

### Added
- Initial project structure.
- Basic server data structures.
- Placeholder for server checking logic.
- Command-line argument parsing.

### Changed
- Initial development iterations.

### Fixed

### Removed
