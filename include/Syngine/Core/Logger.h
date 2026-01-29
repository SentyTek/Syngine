// ╒═══════════════════════════ Logger.h ═╕
// │ Syngine                              │
// │ Created 2025-07-14                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>
#include <signal.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <execinfo.h>
#include <unistd.h>
#include <cxxabi.h>
#endif

#include <filesystem>
#include <fstream>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>

namespace Syngine {
// Forward declaration of Core class
class Core;

/// @brief Log levels for the logger
/// @section Logger
/// @since v0.0.1
enum class LogLevel {
    INFO, //* Informational messages
    WARN, //* Warning messages
    ERR,  //* Error messages
    FATAL, //* Fatal error messages. Will terminate the application
};

/// @brief Logger class for logging messages
/// @section Logger
/// @since v0.0.1
class Logger {
  private:
    Logger() = default; // Private constructor for singleton pattern

    /// @brief Get the current timestamp as a string
    /// @return Current timestamp as a string
    /// @since v0.0.1
    /// @internal
    static std::string _GetTimestamp();

    /// @brief Convert a LogLevel to a string
    /// @param level LogLevel to convert
    /// @return String representation of the LogLevel
    /// @since v0.0.1
    /// @internal
    /// @nodiscard
    [[nodiscard]]
    static std::string _LogLevelToString(LogLevel level) noexcept;

    static inline std::string                    m_appName = "SyngineGame";
    static inline std::unique_ptr<std::ofstream> m_logFile = nullptr;
    static inline std::mutex                     m_logMutex;
    static inline LogLevel                       m_minLogLevel = LogLevel::INFO;
    static inline std::atomic<bool>              m_autoFlush   = false;

    static void _CrashHandler(int signal);
#ifdef _WIN32
    static LONG WINAPI
    _WindowsExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);
#endif

  public:
    /// @brief Initialize the logger with the name of the app and an optional
    /// log path, relative to the app's data directory.
    /// @param appname Name of the application
    /// @param logPath Path to the log file, defaults to "log.txt" in the app's
    /// data directory
    /// @note Should be called once at the start of the application.
    /// @throws Could throw std::runtime_error if the log file cannot be opened
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void Init(const std::string& appname,
                     const std::filesystem::path& logPath = std::filesystem::path("log.txt"));

    /// @brief Shutdown the logger
    /// @note Should be called once at the end of the application.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void Shutdown();

    /// @brief Log a message to disk with an optional log level
    /// @param message Message to log
    /// @param level Log level, defaults to LogLevel::INFO
    /// @param toConsole Whether to log to the console, defaults to true
    /// @threadsafety not-safe
    /// @throws std::runtime_error if the log file is locked or unavailable
    /// @since v0.0.1
    static void Log(const std::string_view message,
                    LogLevel               level     = LogLevel::INFO,
                    bool                   toConsole = true);

    /// @brief Log a formatted message to disk with an optional log level
    /// @param level Log level, defaults to LogLevel::INFO
    /// @param fmt Format string
    /// @param ... Format string arguments
    /// @threadsafety not-safe
    /// @throws std::runtime_error if the log file is locked or unavailable
    /// @since v0.0.1
    static void LogF(LogLevel level, const char* fmt, ...);

    /// @brief Log an error
    /// @param message Error message to log
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void Error(const std::string_view message);

    /// @brief Log an informational message
    /// @param message Informational message to log
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void Info(const std::string_view message);

    /// @brief Show a popup and log an informational message
    /// @param message Informational message to log
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void InfoPopup(const std::string_view message);

    /// @brief Log a warning
    /// @param message Warning message to log
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void Warn(const std::string_view message);

    /// @brief Log a fatal error and shutdown the logger
    /// @param message Fatal error message to log
    /// @note This will terminate the application. If core debug is on, will also pause the app.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void Fatal(const std::string_view message);

    /// @brief Log hardware information
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void LogHardwareInfo();

    /// @brief Set the autoflush behavior. Flushing is good for debug, but can
    /// be bad on performance.
    /// @param enable Whether to enable autoflush
    /// @threadsafety safe
    /// @since v0.0.1
    static inline void SetAutoFlush(bool enable) noexcept {
        m_autoFlush = enable;
    }

    /// @brief Force flush the log file
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void Flush();

    /// @brief Returns true if the log file is open
    /// @return True if the log file is open, false otherwise
    /// @threadsafety safe
    /// @since v0.0.1
    /// @nodiscard
    [[nodiscard]]
    static inline bool IsOpen() noexcept {
        return m_logFile && m_logFile->is_open();
    }

    /// @brief Set the minimum log level. Messages below this level will not be
    /// logged.
    /// @param level Minimum log level to set
    /// @threadsafety safe
    /// @since v0.0.1
    static inline void SetMinLogLevel(LogLevel level) noexcept {
        m_minLogLevel = level;
    }

    /// @brief Get the current minimum log level
    /// @return Current minimum log level
    /// @threadsafety read-only
    /// @since v0.0.1
    static inline LogLevel GetMinLogLevel() noexcept { return m_minLogLevel; }

    /// @brief Setup crash handler to log crashes
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void SetupCrashHandler();

    /// @brief Print the current stack trace to the log
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void PrintStackTrace();
};

} // namespace Syngine