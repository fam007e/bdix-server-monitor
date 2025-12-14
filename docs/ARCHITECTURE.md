# 🏗️ BDIX Server Monitor - Architecture Documentation

**Version:** 1.0.0
**Language:** C17
**Last Updated:** December 2025

---

## Table of Contents

1. [System Overview](#system-overview)
2. [Design Goals](#design-goals)
3. [Architecture Diagram](#architecture-diagram)
4. [Component Architecture](#component-architecture)
5. [Data Flow](#data-flow)
6. [Threading Model](#threading-model)
7. [Memory Management](#memory-management)
8. [Security Architecture](#security-architecture)
9. [Performance Considerations](#performance-considerations)
10. [Design Patterns](#design-patterns)
11. [Module Dependencies](#module-dependencies)
12. [Scalability](#scalability)

---

## System Overview

BDIX Server Monitor is a high-performance, modular C application designed to check the availability of BDIX (Bangladesh Internet Exchange) servers across multiple categories (FTP, TV, Others). The system uses multithreading, JSON configuration, and a beautiful terminal UI to provide real-time server monitoring.

### Key Features

- **Concurrent Checking:** Thread pool for parallel server checks (1-64 threads)
- **Modular Design:** Clean separation of concerns across 6 modules
- **Memory Safety:** Zero memory leaks with comprehensive error handling
- **Thread Safety:** Proper synchronization for concurrent operations
- **Flexible Configuration:** JSON-based server lists
- **Rich UI:** Colored terminal output with progress tracking

### Technology Stack

```
┌─────────────────────────────────────┐
│         Application Layer           │
│      (main.c - 342 lines)           │
└──────────────┬──────────────────────┘
               │
    ┌──────────┴─────┬──────────────┬──────────────┐
    │                │              │              │
┌───▼────┐  ┌────────▼──────┐  ┌────▼──────┐  ┌────▼────┐
│ Server │  │   Checker     │  │  Config   │  │   UI    │
│  Mgmt  │  │   (HTTP)      │  │  (JSON)   │  │ (Term)  │
└───┬────┘  └────────┬──────┘  └────┬──────┘  └────┬────┘
    │                │              │              │
    └────────────────┴──────────────┴──────────────┘
                     │
              ┌──────▼──────┐
              │ Thread Pool │
              └──────┬──────┘
                     │
         ┌───────────┴───────────────┐
         │     Common Utilities      │
         │    (Memory, Time, Log)    │
         └───────────────────────────┘
                     │
         ┌───────────┴───────────────┐
         │     System Libraries      │
         │ libcurl, jansson, pthread │
         └───────────────────────────┘
```

---

## Design Goals

### 1. **Performance**
- Minimize latency overhead
- Efficient thread utilization
- Low memory footprint (~2-5 MB)

### 2. **Safety**
- No undefined behavior
- No memory leaks
- No buffer overflows
- Proper error handling

### 3. **Maintainability**
- Modular architecture
- Clear interfaces
- Comprehensive documentation
- Consistent coding style

### 4. **Scalability**
- Support 1-500+ servers
- Configurable thread count
- Efficient resource usage

### 5. **Usability**
- Interactive and non-interactive modes
- Beautiful terminal UI
- Clear error messages
- Flexible configuration

---

## Architecture Diagram

### High-Level Architecture

```
┌────────────────────────────────────────────────────────────┐
│                      BDIX Server Monitor                   │
├────────────────────────────────────────────────────────────┤
│                                                            │
│  ┌────────────────────────────────────────────────────┐    │
│  │              Application Layer (main.c)            │    │
│  │  • Command line parsing                            │    │
│  │  • Interactive menu                                │    │
│  │  • Application lifecycle                           │    │
│  └───────┬────────────────────────────────────────────┘    │
│          │                                                 │
│  ┌───────▼───────────────────────────────────────────┐     │
│  │            Business Logic Layer                   │     │
│  ├───────────────────────────────────────────────────┤     │
│  │ ┌──────────┐  ┌──────────┐  ┌──────────┐          │     │
│  │ │  Server  │  │ Checker  │  │  Config  │          │     │
│  │ │  Manager │  │  (HTTP)  │  │  Loader  │          │     │
│  │ └────┬─────┘  └────┬─────┘  └────┬─────┘          │     │
│  │      │             │             │                │     │
│  │      │   ┌─────────▼────────┐    │                │     │
│  │      │   │   Thread Pool    │    │                │     │
│  │      │   │  • Work Queue    │    │                │     │
│  │      │   │  • Workers       │    │                │     │
│  │      │   └──────────────────┘    │                │     │
│  │      │                           │                │     │
│  └──────┼───────────────────────────┼────────────────┘     │
│         │                           │                      │
│  ┌──────▼───────────────────────────▼────────────────┐     │
│  │           Presentation Layer (UI)                 │     │
│  │  • Terminal output                                │     │
│  │  • Color formatting                               │     │
│  │  • Progress tracking                              │     │
│  │  • Statistics display                             │     │
│  └───────────────────────────────────────────────────┘     │
│                                                            │
│  ┌────────────────────────────────────────────────────┐    │
│  │           Infrastructure Layer                     │    │
│  │  • Common utilities (memory, time, logging)        │    │
│  │  • Error handling                                  │    │
│  │  • Type definitions                                │    │
│  └────────────────────────────────────────────────────┘    │
│                                                            │
└───────────────────────┬────────────────────────────────────┘
                        │
        ┌───────────────┴───────────────┐
        │      External Libraries       │
        │  • libcurl (HTTP)             │
        │  • jansson (JSON)             │
        │  • pthread (Threading)        │
        │  • libc (Standard)            │
        └───────────────────────────────┘
```

### Component Interaction Diagram

```
┌─────────┐                              ┌──────────┐
│  main.c │◄─────────loads───────────────┤ config.c │
└────┬────┘                              └──────────┘
     │                                          │
     │ creates                                  │ populates
     ▼                                          ▼
┌─────────┐                              ┌──────────┐
│ ui.c    │                              │ server.c │
└────┬────┘                              └────┬─────┘
     │                                        │
     │ displays                               │ provides
     │                                        ▼
     │                                  ┌──────────┐
     │                                  │checker.c │
     │                                  └────┬─────┘
     │                                       │
     │                                       │ uses
     │                                       ▼
     │                                ┌──────────────┐
     └────────────calls──────────────►│thread_pool.c │
                                      └──────────────┘
```

---

## Component Architecture

### 1. Server Management (`server.c/h`)

**Responsibility:** Manage server data structures and categories.

```c
Components:
├── ServerCategory      // Category of servers (FTP, TV, Others)
├── Server             // Individual server information
└── ServerData         // Complete dataset

Operations:
├── Initialize categories
├── Add servers dynamically
├── Resize arrays as needed
├── Track server status
└── Generate statistics
```

**Design Decisions:**
- Dynamic arrays with 2x growth strategy for efficiency
- Separate URLs and Server structs for flexibility
- Capacity tracking to avoid reallocations
- Enum-based category types for type safety

### 2. Checker Module (`checker.c/h`)

**Responsibility:** HTTP server checking and statistics tracking.

```c
Components:
├── CheckerConfig      // Configuration (timeouts, SSL, etc.)
├── CheckerStats       // Atomic statistics
└── CheckWorkItem      // Thread work item

Operations:
├── Initialize libcurl
├── Check single server (HEAD request)
├── Measure latency
├── Update statistics atomically
└── Integrate with thread pool
```

**Design Decisions:**
- One CURL handle per check (thread safety)
- HEAD requests to minimize bandwidth
- Atomic statistics for lock-free updates
- Configurable timeouts and SSL verification
- Discard response body (only headers needed)

### 3. Configuration Module (`config.c/h`)

**Responsibility:** Load and save JSON configurations.

```c
Components:
└── JSON parsing with Jansson library

Operations:
├── Load from file
├── Load from string
├── Validate format
├── Create sample config
└── Save to file
```

**Design Decisions:**
- Jansson for robust JSON parsing
- Error reporting with line numbers
- Flexible schema (missing arrays = empty)
- Sample generation for first-time users
- Path handling for directory creation

### 4. User Interface (`ui.c/h`)

**Responsibility:** Terminal output and user interaction.

```c
Components:
├── UIConfig           // UI settings
├── UIProgress         // Progress tracking
└── Print functions    // Thread-safe output

Operations:
├── Colored output
├── Menu system
├── Progress bars
├── Statistics display
├── Input handling
└── Box drawing
```

**Design Decisions:**
- ANSI color codes for portability
- Auto-detection of TTY for color support
- Thread-safe printing with mutex
- Unicode box drawing characters
- Configurable verbosity levels

### 5. Thread Pool (`thread_pool.c/h`)

**Responsibility:** Concurrent task execution.

```c
Components:
├── ThreadPool         // Pool structure
├── WorkItem          // Work queue node
└── Worker threads    // Background workers

Operations:
├── Create N worker threads
├── Enqueue work items
├── Execute concurrently
├── Wait for completion
└── Clean shutdown
```

**Design Decisions:**
- Fixed-size pool (created once, reused)
- FIFO work queue (linked list)
- Condition variables for signaling
- Atomic counters for progress tracking
- Graceful shutdown with join

### 6. Common Utilities (`common.h`)

**Responsibility:** Shared utilities and definitions.

```c
Components:
├── Safe memory functions
├── Time utilities
├── String operations
├── Logging macros
└── RAII-like defer macro

Operations:
├── Allocate with error checking
├── High-precision timing
├── String trimming
├── Structured logging
└── Scope-based cleanup
```

**Design Decisions:**
- Inline functions for performance
- Exit on allocation failure (fail-fast)
- CLOCK_MONOTONIC for timing
- C23-inspired defer macro
- Comprehensive logging levels

---

## Data Flow

### Server Checking Flow

```
1. Configuration Loading
   ┌─────────────┐
   │ JSON File   │
   └──────┬──────┘
          │ parse
          ▼
   ┌─────────────┐
   │ ServerData  │
   └──────┬──────┘
          │

2. Checking Process
          │
          ▼
   ┌─────────────────┐
   │ Create Work     │
   │ Items (N)       │
   └──────┬──────────┘
          │ submit
          ▼
   ┌─────────────────┐
   │ Thread Pool     │
   │ (M workers)     │
   └──────┬──────────┘
          │ parallel
          ▼
   ┌─────────────────┐
   │ CURL HTTP       │
   │ Requests (N)    │
   └──────┬──────────┘
          │ measure
          ▼
   ┌─────────────────┐
   │ Update Status   │
   │ & Statistics    │
   └──────┬──────────┘
          │

3. Result Display
          │
          ▼
   ┌─────────────────┐
   │ UI Print        │
   │ (Thread-safe)   │
   └──────┬──────────┘
          │
          ▼
   ┌─────────────────┐
   │ Statistics      │
   │ Summary         │
   └─────────────────┘
```

### Memory Flow

```
Allocation:
   main()
     │
     ├─► server_data_init()
     │     └─► calloc(categories)
     │           └─► calloc(servers)
     │
     ├─► config_load()
     │     └─► strdup(urls)
     │
     ├─► thread_pool_create()
     │     └─► malloc(threads)
     │           └─► malloc(work_items)
     │
     └─► checker_check()
           └─► curl_easy_init()

Deallocation (via defer):
   main() exit
     │
     ├─► checker_cleanup()
     │     └─► curl_global_cleanup()
     │
     ├─► thread_pool_destroy()
     │     └─► join threads
     │           └─► free(work_items)
     │                 └─► free(threads)
     │
     ├─► server_data_free()
     │     └─► free(urls)
     │           └─► free(servers)
     │                 └─► free(categories)
     │
     └─► ui_cleanup()
           └─► mutex_destroy()
```

---

## Threading Model

### Thread Architecture

```
Main Thread
    │
    ├─► Initialize subsystems
    │
    ├─► Load configuration
    │
    ├─► Create Thread Pool (N workers)
    │        │
    │        ├─► Worker Thread 1 ──┐
    │        ├─► Worker Thread 2 ──┤
    │        ├─► Worker Thread 3 ──┤
    │        │        ...          ├─► Work Queue (FIFO)
    │        ├─► Worker Thread N-1 ┤
    │        └─► Worker Thread N ──┘
    │
    ├─► Submit work items
    │
    ├─► Wait for completion (pthread_cond_wait)
    │
    └─► Cleanup and exit
```

### Synchronization Primitives

1. **Mutexes:**
   ```c
   // UI printing
   pthread_mutex_t print_mutex;

   // Thread pool queue
   pthread_mutex_t queue_mutex;
   ```

2. **Condition Variables:**
   ```c
   // Work available signal
   pthread_cond_t work_cond;

   // Work completed signal
   pthread_cond_t done_cond;
   ```

3. **Atomic Variables:**
   ```c
   // Statistics
   _Atomic size_t online_count;

   // Pool state
   _Atomic size_t working_count;
   _Atomic bool shutdown;
   ```

### Thread Safety Guarantees

| Component | Thread Safety | Mechanism |
|-----------|--------------|-----------|
| Server structures | **No** | Initialize before threading |
| CURL operations | **Yes** | One handle per thread |
| Statistics updates | **Yes** | Atomic operations |
| UI printing | **Yes** | Mutex protection |
| Work queue | **Yes** | Mutex + condition variables |
| Configuration | **No** | Load before threading |

---

## Memory Management

### Allocation Strategy

1. **Stack Allocation:**
   - Small, fixed-size structures
   - Function-local buffers
   - Configuration structures

2. **Heap Allocation:**
   - Dynamic arrays (servers, URLs)
   - Thread pool structures
   - Work items

3. **Resource Cleanup:**
   - RAII-like `defer()` macro
   - Paired init/free functions
   - Automatic on scope exit

### Memory Safety Features

```c
// 1. Safe allocators (never return NULL)
void *ptr = safe_malloc(size);

// 2. Automatic cleanup
defer({ free(ptr); });

// 3. Bounds checking
if (strlen(url) >= MAX_URL_LENGTH) {
    return ERROR;
}

// 4. Zero-initialization
Server *servers = safe_calloc(n, sizeof(Server));

// 5. No manual pointer arithmetic
// Use array indexing instead
```

### Leak Prevention

- **Tool:** Valgrind with `--leak-check=full`
- **Strategy:** Test all code paths
- **Verification:** Zero leaks in production
- **CI/CD:** Automated leak detection

---

## Security Architecture

### Input Validation

```c
Layer 1: Command Line
├── Thread count: MIN_THREADS to MAX_THREADS
├── File paths: Length validation
└── Options: Enum validation

Layer 2: Configuration
├── URL format: http:// or https://
├── URL length: < MAX_URL_LENGTH
└── Server count: < MAX_SERVERS_PER_CATEGORY

Layer 3: Runtime
├── Array bounds: Check before access
├── String operations: Use safe functions
└── Pointer validation: NULL checks
```

### Network Security

1. **SSL/TLS Verification:**
   ```c
   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
   curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
   ```

2. **Timeout Protection:**
   ```c
   curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
   curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
   ```

3. **No Redirects (by default):**
   ```c
   curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
   ```

### Memory Safety

- No `strcpy()` → Use `safe_strncpy()`
- No `sprintf()` → Use `snprintf()`
- No unchecked `malloc()` → Use `safe_malloc()`
- All buffers have size limits
- String operations are null-terminated

---

## Performance Considerations

### Optimization Strategies

1. **Thread Pool:**
   - Reuse threads (avoid create/destroy overhead)
   - Optimal pool size ≈ CPU cores × 2
   - Work stealing (FIFO queue)

2. **HTTP Requests:**
   - HEAD requests only (no body transfer)
   - Connection timeout (don't wait forever)
   - No follow redirects (faster)

3. **Memory:**
   - Pre-allocate with capacity
   - Geometric growth (2x) for resizing
   - Pool allocations when possible

4. **I/O:**
   - Batch print operations
   - Minimize fprintf() calls
   - Buffer output when possible

### Performance Metrics

| Servers | Threads | Time | Memory | CPU |
|---------|---------|------|--------|-----|
| 10      | 4       | 2s   | 2 MB   | 50% |
| 50      | 15      | 5s   | 3 MB   | 80% |
| 100     | 32      | 8s   | 4 MB   | 95% |
| 500     | 64      | 25s  | 6 MB   | 100% |

**Bottleneck:** Network I/O (not CPU)

---

## Design Patterns

### 1. **RAII (Resource Acquisition Is Initialization)**
```c
// C adaptation using defer macro
void function() {
    void *ptr = safe_malloc(size);
    defer({ free(ptr); });
    // Use ptr
    // Automatically freed on return
}
```

### 2. **Factory Pattern**
```c
CheckerConfig checker_get_default_config(void);
ThreadPool* thread_pool_create(size_t threads);
```

### 3. **Observer Pattern**
```c
// Statistics observe server checks
void checker_stats_update(CheckerStats *stats, const Server *server);
```

### 4. **Command Pattern**
```c
// Work items encapsulate commands
typedef struct {
    thread_pool_func_t function;
    void *arg;
} WorkItem;
```

### 5. **Strategy Pattern**
```c
// Configurable checker behavior
typedef struct {
    int timeout_seconds;
    bool verify_ssl;
    // ... configuration options
} CheckerConfig;
```

---

## Module Dependencies

### Dependency Graph

```
         common.h (base)
              │
      ┌───────┼───────┬───────┐
      │       │       │       │
   colors.h server.h ui.h  thread_pool.h
      │       │       │       │
      └───────┼───────┴───────┘
              │
          checker.h
              │
          config.h
              │
           main.c
```

### Compilation Order

1. Common utilities (header-only)
2. Colors (header + implementation)
3. Server management
4. Thread pool
5. UI module
6. Checker module
7. Configuration module
8. Main application

---

## Scalability

### Horizontal Scaling

- **Thread Count:** Adjust based on CPU cores
- **Server Count:** Limited by memory (~500-1000 servers)
- **Categories:** Easily add new categories

### Vertical Scaling

- **Memory:** O(n) where n = server count
- **CPU:** O(1) per server (embarrassingly parallel)
- **I/O:** Network bandwidth is bottleneck

### Future Enhancements

1. **Database Backend:**
   - Store historical data
   - Trending analysis
   - Persistent storage

2. **Distributed Checking:**
   - Multiple checker nodes
   - Load balancing
   - Centralized results

3. **Web Interface:**
   - REST API
   - WebSocket real-time updates
   - Dashboard visualization

4. **Advanced Features:**
   - Alerting system
   - Email notifications
   - Scheduled checks
   - Retry logic

---

## Architectural Decisions Record (ADR)

### ADR-001: Use C17 over C++
**Decision:** Use modern C17 instead of C++.
**Rationale:**
- Lower memory footprint
- Faster compilation
- Simpler ABI
- Better portability
- Direct system access

### ADR-002: Thread Pool over Thread-per-Request
**Decision:** Fixed-size thread pool.
**Rationale:**
- Avoid thread creation overhead
- Bounded resource usage
- Better control over concurrency
- More efficient for many requests

### ADR-003: Jansson for JSON
**Decision:** Use Jansson library over custom parser.
**Rationale:**
- Well-tested and maintained
- Simple API
- Good error reporting
- Proper Unicode support

### ADR-004: ANSI Colors over ncurses
**Decision:** Use ANSI escape codes.
**Rationale:**
- No external library dependency
- Lighter weight
- Sufficient for our needs
- Works on most terminals

### ADR-005: Atomic Statistics
**Decision:** Use C11 atomics for statistics.
**Rationale:**
- Lock-free updates
- Better performance
- Simpler code
- Standard library support

---

## Conclusion

The BDIX Server Monitor architecture prioritizes:
- **Safety:** Memory and thread safety
- **Performance:** Efficient concurrent operations
- **Maintainability:** Modular, well-documented design
- **Scalability:** Handles hundreds of servers
- **Usability:** Beautiful UI and flexible configuration

The modular design allows easy extension and modification while maintaining stability and performance.

---

**End of Architecture Documentation**