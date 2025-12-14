
# 󰒒 BDIX Server Monitor

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C17](https://img.shields.io/badge/C-17-blue.svg)](https://en.wikipedia.org/wiki/C17_(C_standard_revision))
[![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/fam007e/bdix-server-monitor/actions)

A high-performance, secure, and modular BDIX (Bangladesh Internet Exchange) server monitoring tool written in modern C17. Features multithreaded server checking, real-time latency monitoring, and beautiful terminal UI.

## ✨ Features

- 🔄 **Multithreaded Checking** - Concurrent server testing with configurable thread pools
- 📊 **Real-time Monitoring** - Live latency measurements and status updates
- 📝 **Markdown Export** - Save successful check results to a file
- 🎨 **Beautiful UI** - Color-coded terminal output with progress indicators
- 🔒 **Security First** - Input validation, bounds checking, and secure HTTP
- 📦 **Modular Design** - Clean separation of concerns for maintainability
- ⚡ **High Performance** - Optimized C implementation with minimal overhead
- 🧪 **Well Tested** - Comprehensive unit test coverage

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────┐
│                        main.c                           │
│                   (Entry Point)                         │
└───────────────┬─────────────────────────────────────────┘
                │
      ┌─────────┴────────┬─────────────┬────────────┐
      │                  │             │            │
┌─────▼──────┐    ┌──────▼─────┐  ┌────▼────┐  ┌────▼────┐
│   ui.c     │    │  config.c  │  │server.c │  │checker.c│
│ (Display)  │    │  (Loader)  │  │ (Data)  │  │ (HTTP)  │
└────────────┘    └────────────┘  └─────────┘  └─────┬───┘
                                                     │
                                              ┌──────▼─────────┐
                                              │ thread_pool.c  │
                                              │  (Concurrency) │
                                              └────────────────┘
```

## 🔧 Prerequisites

### Required Dependencies
- **GCC** >= 9.0 or **Clang** >= 10.0
- **libcurl** >= 7.68.0
- **jansson** >= 2.13
- **pthread** (POSIX threads)

### Installation (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential libcurl4-openssl-dev libjansson-dev
```

### Installation (Fedora/RHEL)
```bash
sudo dnf install gcc make libcurl-devel jansson-devel
```

### Installation (macOS)
```bash
brew install curl jansson
```

## 🚀 Quick Start

### 1. Clone the Repository
```bash
git clone https://github.com/fam007e/bdix-server-monitor.git
cd bdix-server-monitor
```

### 2. Build
```bash
make
```

### 3. Run
```bash
./bin/bdix-monitor
```

Or install system-wide:
```bash
sudo make install
bdix-monitor
```

## 📖 Usage

### Interactive Mode
```bash
bdix-monitor
bdix-monitor
```
This will launch an interactive menu in your terminal. Use the arrow keys or number keys to navigate.
- Options 1-4: Check servers.
- Option 9: **Save Results to Markdown**. This will export the list of currently ONLINE servers to a timestamped file (e.g., `bdix_results_20251215_120000.md`).

### Command Line Options
```bash
bdix-monitor [OPTIONS]

Options:
  -c, --config FILE      Use custom config file (default: data/server.json)
  -t, --threads NUM      Number of threads (1-64, default: 15)
  -f, --ftp              Check only FTP servers
  -v, --tv               Check only TV servers
  -o, --others           Check only other servers
  -a, --all              Check all servers (default)
  -q, --quiet            Show only successful checks
  -n, --no-color         Disable colored output
  -i, --interactive      Start in interactive mode (default)
  -s, --stats            Show statistics only
  -h, --help             Show this help message
  -V, --version          Show version information
```

### Examples
```bash
# Check all servers with 32 threads
bdix-monitor --threads 32

# Check only FTP servers, show only successful
bdix-monitor --ftp --quiet

# Use custom config file
bdix-monitor --config /path/to/servers.json
```

## 📁 Configuration

Create `data/server.json`:

```json
{
  "ftp": [
    "http://ftp.amigait.com",
    "http://ftp.dhakacom.net",
    "http://ftp.circleftp.net"
  ],
  "tv": [
    "http://tv.bdix.app",
    "http://iptv.live.net.bd"
  ],
  "others": [
    "http://cache.google.com",
    "http://mirror.dhakacom.com"
  ]
}
```

## 🧪 Development

### Build Debug Version
```bash
make debug
```

### Run Tests
```bash
make tests
make check
```

### Code Formatting
```bash
make format
```

### Static Analysis
```bash
make analyze
```

## 📚 Documentation

- [API Documentation](docs/API.md)
- [Architecture Guide](docs/ARCHITECTURE.md)
- [Contributing Guidelines](CONTRIBUTING.md)

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) first.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📊 Performance

Typical performance on modern hardware:
- **100 servers**: ~2-5 seconds (15 threads)
- **500 servers**: ~10-15 seconds (32 threads)
- **Memory usage**: ~2-5 MB
- **Thread overhead**: Minimal with thread pool

## 🔐 Security

- Input validation on all user inputs
- Bounds checking on arrays and buffers
- SSL/TLS verification enabled
- No arbitrary code execution
- Safe string operations (no strcpy/sprintf)
- Memory sanitization on cleanup

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Original Python version: [0xAhmadYousuf/BDIX](https://github.com/0xAhmadYousuf/BDIX)
- Inspired by the BDIX community in Bangladesh
- Built with ❤️ for network administrators

## 📞 Contact

- **Maintainer**: fam007e
- **Email**: [email](mailto:faisalmoshiur+bdixftp@gmail.com)
- **Issues**: [GitHub Issues](https://github.com/fam007e/bdix-server-monitor/issues)
- **Discussions**: [GitHub Discussions](https://github.com/fam007e/bdix-server-monitor/discussions)
