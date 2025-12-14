# Usage Guide

BDIX Server Monitor can be used in two primary modes: **Interactive Mode** (default) and **CLI Mode** (via arguments).

## Interactive Mode

Simply run the compiled binary:

```bash
./bin/bdix-monitor
```

You will be presented with a menu:

```text
╔════════════════════════════════════════════╗
║           BDIX SERVER MONITOR              ║
╚════════════════════════════════════════════╝
[1] Check FTP Servers
[2] Check TV Servers
[3] Check Other Servers
[4] Check ALL Servers
[5] Set Thread Count (Current: 15)
[6] Toggle Show Only OK (Current: OFF)
[7] View Statistics
[8] Reload Config
[9] Save Results to Markdown
[0] Exit
```

### Key Features
-   **Options 1-4**: Starts checking the specific category of servers. Progress is shown in real-time.
-   **Option 5**: Adjust concurrency. Higher threads = faster checks but higher CPU/Network usage. (Max 64).
-   **Option 9**: Exports the list of *currently online* servers to a timestamped Markdown file (e.g., `bdix_results_20231024_1200.md`). Useful for sharing lists.

## Command Line Interface (CLI)

You can automate checks or run them in single-pass mode using arguments.

### Flags

| Short | Long | Description |
| :--- | :--- | :--- |
| `-c` | `--config FILE` | Path to custom config file (default: `data/server.json`). |
| `-t` | `--threads NUM` | Number of threads to use (1-64). Default is 15. |
| `-f` | `--ftp` | Check ONLY FTP servers. |
| `-v` | `--tv` | Check ONLY TV servers. |
| `-o` | `--others` | Check ONLY other servers. |
| `-a` | `--all` | Check ALL servers (implicit if no other check mode is set). |
| `-q` | `--quiet` | Quiet mode. Only prints servers that are **ONLINE**. Hides offline/errors. |
| `-n` | `--no-color` | Disable ANSI color output (useful for logging to files). |
| `-i` | `--interactive` | Force interactive mode (default behavior). |
| `-s` | `--stats` | Show loaded server statistics and exit. |
| `-h` | `--help` | Show help message. |

### Examples

**1. Quick check of all servers (quietly)**
```bash
./bin/bdix-monitor --all --quiet
```

**2. Check TV servers with high concurrency**
```bash
./bin/bdix-monitor --tv --threads 32
```

**3. Use a different server list**
```bash
./bin/bdix-monitor --config /home/user/my_custom_servers.json
```

**4. Save output to a file (plain text)**
```bash
./bin/bdix-monitor --all --no-color > results.txt
```
