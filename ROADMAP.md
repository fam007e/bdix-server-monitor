# ROADMAP

This document outlines the future development plans and potential features for the BDIX Server Monitor. This roadmap is subject to change based on community feedback, development resources, and evolving requirements.

## 🚀 Vision

To provide a robust, highly performant, and user-friendly tool for monitoring BDIX servers, offering deep insights into their status and performance. We aim to make it a go-to solution for network administrators and enthusiasts in the BDIX community.

## 🎯 Near-Term Goals (Next 1-3 Months)

*   **Enhanced Reporting**:
    *   Export check results to CSV/JSON for external analysis (Markdown export implemented).
    *   Generate summary reports (daily/weekly) of server performance.
*   **Persistent Configuration**:
    *   Allow saving current application settings (e.g., thread count, display options) to a configuration file.
*   **Improved Error Handling**:
    *   More detailed error messages from `libcurl` and `jansson` to aid debugging.
    *   Introduce configurable retry mechanisms for failed checks.
*   **Community Engagement**:
    *   Actively solicit feedback on new features and improvements.
    *   Improve documentation based on user questions.

## 🌱 Mid-Term Goals (Next 3-6 Months)

*   **Notification System**:
    *   Integrate with popular notification services (e.g., Discord, Telegram, Email) for critical server status changes.
    *   Configurable alert thresholds (e.g., latency spikes, extended downtime).
*   **Historical Data Storage**:
    *   Implement a lightweight, local database (e.g., SQLite) to store historical check data.
    *   Allow viewing trends and past performance within the application.
*   **Advanced UI Features**:
    *   Interactive terminal dashboard with live updates without manual refresh.
    *   Configurable display columns for server lists.
*   **IPv6 Support**:
    *   Ensure full compatibility and testing for IPv6-only or dual-stack servers.

## 🌳 Long-Term Goals (6+ Months)

*   **Web-based Dashboard**:
    *   Develop a simple, optional web interface for remote monitoring and visualization.
    *   REST API for programmatic access to server status and historical data.
*   **Distributed Monitoring**:
    *   Ability to deploy multiple checker instances and aggregate results centrally.
*   **Plugin Architecture**:
    *   Allow community-contributed modules for custom check types or integrations.
*   **Cross-Platform Binaries**:
    *   Provide pre-compiled binaries for popular Linux distributions, macOS, and potentially Windows.

## 🤝 How You Can Help

Your contributions are invaluable!
*   **Feedback**: Share your thoughts on proposed features, report bugs, and suggest new ideas.
*   **Code**: Help implement features, fix bugs, or improve existing code. See `CONTRIBUTING.md`.
*   **Documentation**: Improve and expand the project documentation.

Join the discussions on [GitHub Discussions](https://github.com/fam007e/bdix-server-monitor/discussions) to share your ideas and insights!
