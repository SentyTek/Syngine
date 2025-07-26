#pragma once
#include "SyngineCore.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>

namespace Syngine {

// Log levels for the logger
enum class LogLevel {
    INFO,
    WARN,
    ERR,
    FATAL,
};

// Logger class for logging messages
class Logger {
  private:
    Logger() = default; // Private constructor for singleton pattern

    static std::string GetTimestamp();

    [[nodiscard]]
    static std::string LogLevelToString(LogLevel level) noexcept;

    static inline std::string appName = "SyngineGame";
    static inline std::unique_ptr<std::ofstream> logFile = nullptr;
    static inline std::mutex                     logMutex;
    static inline LogLevel                       minLogLevel = LogLevel::INFO;
    static inline std::atomic<bool>              autoFlush   = false;

  public:
    // Initialize the logger with the name of the app and an optional log path,
    // relative to the app's data directory.
    // Should be called once at the start of the application.
    static void Init(const std::string& appname,
                     const std::filesystem::path& logPath = std::filesystem::path("log.txt"));

    // Shutdown and flush the logger
    static void Shutdown();

    // Log a message to disk with an optional log level
    static void Log(const std::string_view message,
                    LogLevel           level     = LogLevel::INFO,
                    bool               toConsole = true);
    // Log a formatted message to disk with a log level
    static void LogF(LogLevel level, const char* fmt, ...);
    // Log an error
    static void Error(const std::string_view message);
    // Log informational message
    static void Info(const std::string_view message);
    // Log a warning
    static void Warn(const std::string_view message);
    // Log a fatal error and shutdown the logger
    static void Fatal(const std::string_view message);

    // Log hardware info
    static void LogHardwareInfo(Syngine::Core& core);

    // Set the autoflush behavior. Flushing is good for debug, but can be bad on performance.
    static inline void SetAutoFlush(bool enable) noexcept {
        autoFlush = enable;
    }

    // Force flush the log file
    static void Flush();

    // Returns true if the log file is open
    [[nodiscard]]
    static inline bool IsOpen() noexcept {
        return logFile && logFile->is_open();
    }
};

} // namespace Syngine