# Building BDIX Server Monitor

This guide covers how to build the BDIX Server Monitor from source on Linux, macOS, and Windows.

## Prerequisites

### General Requirements
-   **C Compiler**: GCC >= 9.0 or Clang >= 10.0 (C17 support required)
-   **Build System**: Make or CMake >= 3.10
-   **Dependencies**:
    -   `libcurl` (development headers)
    -   `jansson` (JSON parsing library)
    -   `pthread` (POSIX threads, usually present by default on UNIX-like systems)

## Platform Specifics

### 🐧 Linux

#### Ubuntu / Debian
```bash
sudo apt update
sudo apt install build-essential libcurl4-openssl-dev libjansson-dev cmake
```

#### Fedora / RHEL
```bash
sudo dnf install gcc make cmake libcurl-devel jansson-devel
```

### 🍎 macOS

Install dependencies via [Homebrew](https://brew.sh/):

```bash
brew install curl jansson cmake
```
*Note: macOS comes with `curl` but installing via Homebrew ensures you have the latest version and headers.*

### 🪟 Windows

We recommend using **MSYS2** to provide a POSIX-like environment and MinGW-w64 compiler. This ensures compatibility with POSIX headers (like `unistd.h` and `getopt.h`) used in the project.

1.  **Install [MSYS2](https://www.msys2.org/)**.
2.  Open the **MSYS2 MinGW 64-bit** terminal.
3.  Install dependencies:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make mingw-w64-x86_64-curl mingw-w64-x86_64-jansson
```

## Compilation

### Using Makefile (Recommended for Linux/macOS)

The simplest way to build is using the provided `Makefile`.

```bash
# Build release version (Optimized)
make

# Build debug version (Symbols enabled, no optimization)
make debug
```

The binary will be created at `./bin/bdix-monitor`.

### Using CMake (Recommended for Cross-Platform/Windows)

CMake is preferred if you need more control or are building on Windows via MSYS2.

```bash
mkdir build
cd build

# Configure (Release build)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
cmake --build .
# OR simply
make
```

### Verifying the Build

Run the binary to check the version:

```bash
./bin/bdix-monitor --version
# Output: BDIX Server Monitor v1.0.0 ...
```
