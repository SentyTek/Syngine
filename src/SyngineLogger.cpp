#include "SDL3/SDL_messagebox.h"
#include "SyngineLogger.h"
#include "SDL3/SDL.h"
#include <fstream>
#include "FsUtils.h"

namespace Syngine {

std::string Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char        buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    return std::string(buf);
}

std::string Logger::LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERR:   return "ERR";
        case LogLevel::FATAL: return "FATAL";
        default:              return "UNKNOWN";
    }
}

void Logger::SetAutoFlush(bool enable) { autoFlush = enable; }

void Logger::Init(const std::string& appname,
                  const std::filesystem::path& logPath) {
    appName = appname;
    std::filesystem::path logFolder = Syngine::GetAppdataPath(appName) / "logs";
    if (logFolder.empty()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Log folder path is empty, cannot initialize logger.");
        return;
    }

    // Ensure the log folder exists
    try {
        std::filesystem::create_directories(logFolder);
    } catch (const std::filesystem::filesystem_error& e) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create log directory: %s", e.what());
        return;
    }

    // Locate previous log if it exists, rename it to `prev.txt`
    std::filesystem::path fullLogPath = logFolder / logPath;
    std::filesystem::path prevLogPath = logFolder / "prev.txt";
    try {
        if (std::filesystem::exists(fullLogPath)) {
            std::filesystem::rename(fullLogPath, prevLogPath);
        }
    } catch (const std::filesystem::filesystem_error& e) {
            SDL_Log("Failed to rename previous log file: %s", e.what());
    }

    // Open the log file for writing and clear its contents
    logFile = std::make_unique<std::ofstream>(fullLogPath,
                                              std::ios::out | std::ios::trunc);
    if (!logFile->is_open()) {
        // Log file probably doen't exist, try to create it
        std::ofstream testFile(fullLogPath);
        if (!testFile.is_open()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create log file at %s", fullLogPath.string().c_str());
            logFile = nullptr;
            return;
        }
        testFile.close();

        // Try opening again
        logFile = std::make_unique<std::ofstream>(
            fullLogPath, std::ios::out | std::ios::trunc);
        if (!logFile->is_open()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open log file at %s", fullLogPath.string().c_str());
            logFile = nullptr;
            return;
        }
    }

    Logger::Log("Logger initialized for " + appName);
    Logger::Log("Log file located at: " + fullLogPath.string());
    Logger::Log("Previous log (if any) moved to: " + prevLogPath.string());

    return;
}

void Logger::Shutdown() {
    if (logFile && logFile->is_open()) {
        // The mutex is already locked if called from Log(FATAL),
        // so we write directly to the file instead of calling Log().
        (*logFile) << "[" << GetTimestamp() << "] "
                   << "[" << LogLevelToString(LogLevel::INFO) << "] "
                   << "Logger shutting down." << std::endl;
        logFile->flush();
        logFile->close();
        logFile = nullptr;
    }
}

void Logger::Log(const std::string& message, LogLevel level, bool toConsole) {
    if (!logFile || !logFile->is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Log file is not open. Message: %s", message.c_str());
        return;
    }

    std::lock_guard<std::mutex> lock(logMutex);
    (*logFile) << "[" << GetTimestamp() << "] "
               << "[" << LogLevelToString(level) << "] "
               << message << std::endl;
    if (autoFlush) {
        logFile->flush();
    }
    if (toConsole) {
        switch (level) {
            case LogLevel::INFO:
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
                break;
            case LogLevel::WARN:
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
                break;
            case LogLevel::ERR: 
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
                break;
            case LogLevel::FATAL:
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
                break;
            default:
                SDL_Log("%s", message.c_str());
                break;
        }
    }

    if (level == LogLevel::FATAL) {
        Shutdown();
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION, "Fatal error: %s", message.c_str());
        std::string finalMessage = appName + " has encountered a fatal error and needs to close:\n\n" + message;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", finalMessage.c_str(), nullptr);
        exit(EXIT_FAILURE);
    }
}

void Logger::LogHardwareInfo(Syngine::Core& core) {
    if (!logFile || !logFile->is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Log file is not open.");
        return;
    }

    Logger::Log("System Specifications:\n" + core.GetSystemSpecifications(), LogLevel::INFO);
}

void Logger::LogF(LogLevel level, const char* fmt, ...) {
    char buffer[1024];

    va_list args;
    va_start(args, fmt);

    int len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    std::string message;
    if (len < static_cast<int>(sizeof(buffer))) {
        message = buffer;
    } else {
        // Allocate large enough heap buffer
        std::vector<char> heapBuf(len + 1);
        va_start(args, fmt);
        vsnprintf(heapBuf.data(), heapBuf.size(), fmt, args);
        va_end(args);
        message = std::string(heapBuf.data());
    }

    Log(message, level);
}

void Logger::Error(const std::string& message) { Log(message, LogLevel::ERR); }
void Logger::Info(const std::string& message) { Log(message, LogLevel::INFO); }
void Logger::Warn(const std::string& message) { Log(message, LogLevel::WARN); }
void Logger::Fatal(const std::string& message) { Log(message, LogLevel::FATAL); }

} // namespace Syngine
