// ╒═══════════════════════════ Logger.h ═╕
// │ Syngine                              │
// │ Created 2025-07-14                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
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

struct SDL_Window; // Forward declaration of SDL_Window

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

    static inline std::string                    m_appName;
    static inline std::unique_ptr<std::ofstream> m_logFile;
    static inline std::mutex                     m_logMutex;
    static inline LogLevel                       m_minLogLevel;
    static inline std::atomic<bool>              m_autoFlush;
    static inline bool                           m_verbose;
    static inline SDL_Window*                    m_mainWindow = nullptr;

    static void _CrashHandler(int signal);

    /// @brief Set the main window for error dialogs
    /// @param window SDL window pointer
    /// @threadsafety safe
    /// @since v0.0.1
    static inline void _SetMainWindow(SDL_Window* window) noexcept {
        m_mainWindow = window;
    }
#ifdef _WIN32
    static LONG WINAPI
    _WindowsExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);
#endif
    static void _Init(
        const std::string&           appname,
        const std::filesystem::path& logPath = std::filesystem::path("log.txt"),
        bool                         verbose = false);

    static void _Shutdown();

    friend class Core;
    friend class Window;
  public:
    /// @brief Log a message to disk with an optional log level
    /// @param message Message to log
    /// @param level Log level, defaults to LogLevel::INFO
    /// @param toConsole Whether to log to the console, defaults to true
    /// @param writeOnlyInDebug When true, only in debug mode will this message
    /// be written to disk. In release mode, it will be ignored. Defaults to
    /// false.
    /// @threadsafety not-safe
    /// @throws std::runtime_error if the log file is locked or unavailable
    /// @since v0.0.1
    static void Log(const std::string_view message,
                    LogLevel               level            = LogLevel::INFO,
                    bool                   writeOnlyInDebug = true,
                    bool                   toConsole        = true);

    /// @brief Log a formatted message to disk with an optional log level
    /// @param level Log level, defaults to LogLevel::INFO
    /// @param writeOnlyInDebug When true, only in debug mode will this message
    /// be written to disk. In release mode, it will be ignored. Defaults to
    /// false.
    /// @param fmt Format string
    /// @param ... Format string arguments
    /// @threadsafety not-safe
    /// @throws std::runtime_error if the log file is locked or unavailable
    /// @since v0.0.1
    static void LogF(LogLevel level, bool writeOnlyInDebug, const char* fmt, ...);

    /// @brief Log an error
    /// @param message Error message to log
    /// @param writeOnlyInDebug When true, only in debug mode will this message
    /// be written to disk. In release mode, it will be ignored. Defaults to
    /// false.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void Error(const std::string_view message, bool writeOnlyInDebug = false);

    /// @brief Log an informational message
    /// @param message Informational message to log
    /// @param writeOnlyInDebug When true, only in debug mode will this message
    /// be written to disk. In release mode, it will be ignored. Defaults to
    /// false.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void Info(const std::string_view message, bool writeOnlyInDebug = false);

    /// @brief Show a popup and log an informational message
    /// @param message Informational message to log
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void InfoPopup(const std::string_view message);

    /// @brief Log a warning
    /// @param message Warning message to log
    /// @param writeOnlyInDebug When true, only in debug mode will this message
    /// be written to disk. In release mode, it will be ignored. Defaults to
    /// false.
    /// @threadsafety not-safe
    /// @since v0.0.1
    static void Warn(const std::string_view message, bool writeOnlyInDebug = false);

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
    static void Flush();    /// @brief Returns true if the log file is open
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

    /// @brief Set the verbose mode. When verbose, certain messages that are normally only logged in debug mode will also be logged in release mode.
    /// @param verbose Whether to enable verbose mode
    /// @threadsafety safe
    /// @since v0.0.1
    static void SetVerbose(bool verbose);

    /// @brief Get whether verbose mode is enabled
    /// @return True if verbose mode is enabled, false otherwise
    /// @threadsafety safe
    /// @since v0.0.1
    static bool IsVerbose();
};

} // namespace Syngine
