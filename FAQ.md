# Frequently Asked Questions (FAQ)

This document addresses common questions about the BDIX Server Monitor.

---

## General

### Q: What is BDIX Server Monitor?
A: BDIX Server Monitor is a high-performance C application designed to check the availability and latency of servers, particularly those related to the Bangladesh Internet Exchange (BDIX). It supports multithreaded checks, JSON configuration, and provides a clear terminal UI.

### Q: Why C?
A: The project is written in modern C17 to achieve high performance, low memory footprint, and efficient resource utilization, especially for concurrent network operations.

### Q: What are the main features?
A: Key features include multithreaded server checking, real-time latency monitoring, color-coded terminal UI, modular design, and robust error handling.

### Q: Where can I get the latest version?
A: You can clone the repository from [GitHub](https://github.com/fam007e/bdix-server-monitor). Refer to the `README.md` for build and installation instructions.

---

## Installation & Setup

### Q: What are the prerequisites to build this project?
A: You need `GCC` (>= 9.0) or `Clang` (>= 10.0), `libcurl` (>= 7.68.0), `jansson` (>= 2.13), and `pthread`. Detailed installation steps for various operating systems are provided in `README.md`.

### Q: How do I compile the project?
A: After cloning the repository, navigate to the project directory and run `make`. For more options, see the `Makefile` or `CMakeLists.txt`.

### Q: How do I install it system-wide?
A: After building, you can run `sudo make install`. This will typically copy the executable to `/usr/local/bin`.

---

## Configuration

### Q: How do I specify which servers to monitor?
A: Servers are configured via a `data/server.json` file. This JSON file specifies different categories (ftp, tv, others) with lists of URLs. A sample `server.json` is provided in `README.md`.

### Q: Can I use a custom configuration file?
A: Yes, you can specify a custom configuration file using the `-c` or `--config` command-line option:
```bash
./bin/bdix-monitor --config /path/to/my_servers.json
```

### Q: What if `data/server.json` doesn't exist?
A: The application will attempt to create a sample `data/server.json` if it's missing and you try to load it. You should then edit this sample file with your desired server URLs.

---

## Usage

### Q: How do I run the monitor in interactive mode?
A: Simply run the executable without any arguments:
```bash
./bin/bdix-monitor
```
This will launch an interactive menu in your terminal.

### Q: How can I check only specific server categories?
A: Use the `-f`, `-v`, or `-o` flags for FTP, TV, or Others categories respectively:
```bash
./bin/bdix-monitor --ftp
./bin/bdix-monitor --tv --quiet # Check TV servers, only show successful ones
```

### Q: How do I change the number of threads used for checking?
A: Use the `-t` or `--threads` option:
```bash
./bin/bdix-monitor --threads 32
```
The thread count is configurable between 1 and 64.

### Q: What do the different status codes mean?
A:
*   **Online**: Server responded with an HTTP status code 2xx or 3xx.
*   **Offline**: Server responded with an HTTP status code 4xx or 5xx, or other non-connection errors.
*   **Timeout**: The connection or request timed out.
*   **Error**: Other network-related errors (e.g., DNS resolution failure).

---

## Development

### Q: How can I contribute to the project?
A: We welcome contributions! Please see the `CONTRIBUTING.md` file for detailed guidelines on how to report bugs, suggest enhancements, and submit pull requests.

### Q: How do I run the tests?
A: After building the project, run `make check` to execute the unit test suite.

### Q: Where can I find more technical details about the architecture or API?
A: Detailed documentation is available in the `docs/` directory:
*   `docs/ARCHITECTURE.md` for system architecture overview.
*   `docs/API.md` for C API reference.

---

## Contact & Support

### Q: I found a bug or have a feature request. Where should I report it?
A: Please use the [GitHub Issues](https://github.com/fam007e/bdix-server-monitor/issues) page to report bugs or suggest new features.

### Q: I have a general question or want to discuss something.
A: You can use [GitHub Discussions](https://github.com/fam007e/bdix-server-monitor/discussions) for general questions, ideas, or community discussions.

### Q: How can I contact the maintainer?
A: You can reach the maintainer, Faisal Ahmed Moshiur, via [email](mailto:faisalmoshiur+bdixftp@gmail.com).

---
