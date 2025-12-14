# 🔒 Security Policy

## Our Commitment

The BDIX Server Monitor team takes the security of our software seriously. We appreciate your efforts to responsibly disclose your findings and will make every effort to acknowledge your contributions.

---

## 📋 Table of Contents

1. [Supported Versions](#supported-versions)
2. [Reporting a Vulnerability](#reporting-a-vulnerability)
3. [Security Best Practices](#security-best-practices)
4. [Known Security Considerations](#known-security-considerations)
5. [Security Features](#security-features)
6. [Disclosure Policy](#disclosure-policy)
7. [Security Updates](#security-updates)
8. [Security Hall of Fame](#security-hall-of-fame)

---

## 📦 Supported Versions

We currently support the following versions with security updates:

| Version | Supported          | Support Status |
| ------- | ------------------ | -------------- |
| 1.0.x   | ✅ Yes            | Active development |
| < 1.0   | ❌ No             | No longer supported |

**Note:** We recommend always using the latest stable release for the best security and features.

---

## 🚨 Reporting a Vulnerability

### Please Do NOT:

- ❌ Open a public GitHub issue for security vulnerabilities
- ❌ Disclose the vulnerability publicly before it's been addressed
- ❌ Exploit the vulnerability beyond what's necessary to demonstrate it

### Please DO:

- ✅ Report security vulnerabilities privately
- ✅ Provide detailed information to help us understand and reproduce the issue
- ✅ Give us reasonable time to address the issue before public disclosure
- ✅ Follow responsible disclosure practices

### How to Report

**Email:** [email](mailto:faisalmoshiur+bdixftp@gmail.com)

**Subject Line:** `[SECURITY] Brief description of vulnerability`

**PGP Key:** (Not available yet)

### What to Include in Your Report

Please provide the following information:

```markdown
**Vulnerability Type:**
[e.g., Buffer Overflow, SQL Injection, Memory Leak, etc.]

**Affected Component:**
[e.g., Checker module, Thread pool, Configuration parser]

**Affected Versions:**
[e.g., 1.0.0 to 1.0.3]

**Severity:**
[Critical / High / Medium / Low]

**Description:**
[Detailed description of the vulnerability]

**Steps to Reproduce:**
1. [First Step]
2. [Second Step]
3. [etc.]

**Proof of Concept:**
[Code or commands demonstrating the vulnerability]

**Impact:**
[What can an attacker do? What data is at risk?]

**Suggested Fix:**
[If you have suggestions, please share them]

**Your Environment:**
- OS: [e.g., Ubuntu 22.04]
- Compiler: [e.g., GCC 11.3]
- Version: [e.g., 1.0.2]
```

### Response Timeline

We aim to:

- **Acknowledge receipt** within 24-48 hours
- **Provide initial assessment** within 5 business days
- **Keep you informed** of progress every 7 days
- **Fix critical issues** within 30 days when possible
- **Coordinate disclosure** with you

### What Happens Next

1. We acknowledge receipt of your report
2. We verify and reproduce the vulnerability
3. We develop a fix and test it thoroughly
4. We prepare a security advisory
5. We release the fix in a new version
6. We publicly disclose the vulnerability (coordinated with you)
7. We credit you in our Security Hall of Fame (if you wish)

---

## 🛡️ Security Best Practices

### For Users

#### 1. Keep Software Updated
```bash
# Check for updates regularly
git pull origin main
make clean && make
```

#### 2. Use Secure Configurations
```bash
# Always verify SSL certificates (default behavior)
./bin/bdix-monitor --all

# Don't disable SSL verification unless absolutely necessary
# If you must, understand the risks
```

#### 3. Protect Configuration Files
```bash
# Set proper file permissions
chmod 600 data/server.json

# Don't store sensitive data in config files
# Use environment variables for secrets
```

#### 4. Run with Least Privilege
```bash
# Don't run as root
./bin/bdix-monitor --all

# Use dedicated user account for production
sudo -u bdix-monitor ./bin/bdix-monitor
```

#### 5. Monitor and Log
```bash
# Enable logging
./bin/bdix-monitor --all 2>&1 | tee monitor.log

# Review logs regularly
grep ERROR monitor.log
```

### For Developers

#### 1. Memory Safety
```c
// Always validate input
if (!ptr || size == 0) {
    return BDIX_ERROR_INVALID_INPUT;
}

// Use safe functions
safe_strncpy(dest, src, sizeof(dest));

// Check bounds
if (index >= array_size) {
    return BDIX_ERROR;
}
```

#### 2. Input Validation
```c
// Validate URLs
if (!is_valid_url(url)) {
    LOG_ERROR("Invalid URL format");
    return BDIX_ERROR_INVALID_INPUT;
}

// Validate lengths
if (strlen(input) >= MAX_INPUT_LENGTH) {
    LOG_ERROR("Input too long");
    return BDIX_ERROR_INVALID_INPUT;
}

// Sanitize input
trim_string(user_input);
```

#### 3. Thread Safety
```c
// Protect shared data
pthread_mutex_lock(&mutex);
// ... critical section ...
pthread_mutex_unlock(&mutex);

// Use atomics for counters
_Atomic size_t counter;
atomic_fetch_add(&counter, 1);
```

#### 4. Error Handling
```c
// Always check return values
if (function() != BDIX_SUCCESS) {
    LOG_ERROR("Function failed");
    cleanup_resources();
    return BDIX_ERROR;
}

// Use defer for cleanup
void *ptr = safe_malloc(size);
defer({ free(ptr); });
```

#### 5. Secure Dependencies
```bash
# Keep dependencies updated
sudo apt update && sudo apt upgrade libcurl4-openssl-dev libjansson-dev

# Verify checksums when downloading
sha256sum library.tar.gz
```

---

## ⚠️ Known Security Considerations

### Network Operations

**Consideration:** The application makes outbound HTTP/HTTPS requests.

**Mitigation:**
- SSL verification is enabled by default
- Connection timeouts prevent indefinite hangs
- No automatic redirect following (prevents SSRF)
- User controls which servers to check

**User Action:** Ensure you trust the servers in your configuration file.

### Configuration Files

**Consideration:** Configuration files are read from disk.

**Mitigation:**
- JSON parser validates format
- URL validation before use
- Bounds checking on all inputs

**User Action:**
- Set proper file permissions (600 or 640)
- Don't store sensitive information in config
- Validate config files before use

### Thread Pool

**Consideration:** Multiple threads operate concurrently.

**Mitigation:**
- Thread count is bounded (1-64)
- Proper synchronization with mutexes
- Atomic operations for counters
- No shared state between checks

**User Action:** Don't set thread count excessively high.

### Memory Management

**Consideration:** Manual memory management in C.

**Mitigation:**
- All allocations are checked
- RAII-like cleanup with defer
- Valgrind testing in CI/CD
- Bounds checking on arrays

**User Action:** Report any crashes immediately.

---

## 🔐 Security Features

### Input Validation

✅ **URL Validation**
- Format checking (http:// or https://)
- Length limits (< 512 characters)
- Character validation

✅ **Bounds Checking**
- Array index validation
- Buffer size limits
- String length checks

✅ **Thread Count Validation**
- Range checking (1-64)
- Resource limit awareness

### Network Security

✅ **SSL/TLS Support**
- Enabled by default
- Certificate verification
- Modern cipher suites

✅ **Timeout Protection**
- Connection timeout: 5 seconds
- Request timeout: 10 seconds
- No indefinite waits

✅ **Request Controls**
- HEAD requests only (no body download)
- No automatic redirects
- Limited to configured servers

### Memory Safety

✅ **Safe String Operations**
- No strcpy() or sprintf()
- All operations bounds-checked
- Null-termination guaranteed

✅ **Resource Cleanup**
- Automatic with defer macro
- Paired init/free functions
- No memory leaks (Valgrind verified)

✅ **Error Handling**
- All allocations checked
- Graceful degradation
- Proper error propagation

### Thread Safety

✅ **Synchronization**
- Mutex-protected critical sections
- Atomic operations for counters
- No race conditions

✅ **Resource Isolation**
- One CURL handle per thread
- No shared mutable state
- Thread-local storage

---

## 📢 Disclosure Policy

### Our Commitment

We follow **Coordinated Vulnerability Disclosure**:

1. **Receive** vulnerability reports privately
2. **Verify** and assess the issue
3. **Develop** a fix
4. **Notify** affected users
5. **Release** patched version
6. **Disclose** details publicly (after fix is available)

### Timeline

- **Critical vulnerabilities:** 0-7 days
- **High severity:** 7-30 days
- **Medium severity:** 30-90 days
- **Low severity:** 90+ days

We will work with you to determine an appropriate disclosure timeline.

### Public Disclosure

After a fix is released:

1. We publish a security advisory
2. We update this document
3. We credit the reporter (if they wish)
4. We notify users through:
   - GitHub Security Advisory
   - Release notes
   - Mailing list (if applicable)

---

## 🔄 Security Updates

### How We Notify Users

- **GitHub Security Advisories** - Critical/High severity
- **Release Notes** - All security fixes
- **CHANGELOG.md** - Detailed changes
- **Email** - Registered users (if applicable)

### How to Stay Informed

1. **Watch** the repository on GitHub
2. **Star** the project for updates
3. **Subscribe** to releases
4. **Check** CHANGELOG.md regularly

### Update Process

```bash
# Check current version
./bin/bdix-monitor --version

# Update to latest
git pull origin main
make clean && make

# Verify update
./bin/bdix-monitor --version
```

---

## 🏆 Security Hall of Fame

We recognize and thank security researchers who help us keep BDIX Server Monitor secure:

| Date | Reporter | Vulnerability | Severity |
|------|----------|--------------|----------|
| - | - | - | - |

**Want to be listed?** Report a valid security vulnerability!

---

## 📚 Additional Resources

### Security Standards

- [OWASP Top 10](https://owasp.org/www-project-top-ten/)
- [CWE Top 25](https://cwe.mitre.org/top25/)
- [SEI CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c)

### Tools

- [Valgrind](https://valgrind.org/) - Memory error detection
- [AddressSanitizer](https://github.com/google/sanitizers) - Memory error detector
- [Cppcheck](http://cppcheck.sourceforge.net/) - Static analysis

### Documentation

- [API Security](docs/API.md#security)
- [Architecture Security](docs/ARCHITECTURE.md#security-architecture)
- [Contributing Guidelines](CONTRIBUTING.md)

---

## 📞 Contact

**Security Team:** [email](mailto:faisalmoshiur+bdixftp@gmail.com)
**General Inquiries:** [email](mailto:faisalmoshiur+bdixftp@gmail.com)
**PGP Key:** (Not available yet)

**Response Time:** We aim to respond within 24-48 hours.

---

## 🙏 Thank You

Thank you for helping keep BDIX Server Monitor and its users safe!

Your responsible disclosure helps protect the entire community.

---

**Version:** 1.0
**Last Updated:** December 2025
**Next Review:** June 2026

---

## Legal

This security policy is provided in good faith. The BDIX Server Monitor team reserves the right to modify this policy at any time. Security researchers are expected to follow ethical disclosure practices and applicable laws.

**This software is provided "as is" without warranty of any kind. See the [LICENSE](LICENSE) file for details.**