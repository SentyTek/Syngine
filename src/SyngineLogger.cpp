#ifdef _WIN32
#define NOMINMAX
#include <ShlObj.h>
#include <knownfolders.h>
#include <winerror.h>

#elif __APPLE__
#include "CoreServices/CoreServices.h"
#include <iostream>

#else
#include <iostream>
#include <cstdlib>

#endif

#include "SyngineLogger.h"
#include "SDL3/SDL.h"
#include <fstream>

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

// Gets the path to the log folder, creates it if it doesn't exist
// Path depends on OS, and boy do I love platform-specific code
std::filesystem::path Logger::GetLogFolderPath() {
    std::filesystem::path logFolder;
#ifdef _WIN32
    // On Windows, use the user's Roaming AppData folder.
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path);
    if (SUCCEEDED(hr)) {
        // Compose the log folder path: AppData\Roaming\SentyTek\<appName>\logs
        logFolder = std::filesystem::path(path) / "SentyTek" / appName / "logs";
        CoTaskMemFree(path);
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get AppData path for log.");
    }

#elif __APPLE__
    // On macOS, use the user's Application Support folder.
    FSRef ref;
    OSType folderType = kApplicationSupportFolderType;
    char   path[PATH_MAX];

    // Find the Application Support folder.
    OSStatus status =
        FSFindFolder(kUserDomain, folderType, kCreateFolder, &ref);
    if (status != noErr) {
        std::cerr << "Error finding Application Support folder: " << status
                  << std::endl;
        return "";
    }
    // Convert FSRef to a filesystem path.
    if (FSRefMakePath(&ref, (UInt8*)path, sizeof(path) != noErr)) {
        std::cerr << "Error converting FSRef to path." << std::endl;
        return "";
    }

    // Compose the log folder path: ~/Library/Application Support/SentyTek/<appName>/logs
    logFolder = std::filesystem::path(path) / "SentyTek" / appName / "logs";

#else
    // On Linux and other platforms, use XDG_DATA_HOME.
    const char* homeDir = std::getenv("XDG_DATA_HOME");

    if (!homeDir) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "XDG_DATA_HOME not set for log path.");
    }

    // Compose the log folder path: $XDG_DATA_HOME/SentyTek/<appName>/logs (or ~/.local/share/SentyTek/<appName>/logs)
    logFolder = std::filesystem::path(homeDir) / "SentyTek" / appName / "logs";
    
#endif
    // Log an error if the path could not be determined.
    if (logFolder.empty()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to determine log folder path.");
    }
    return logFolder;
}

void Logger::SetAutoFlush(bool enable) { autoFlush = enable; }

void Logger::Init(const std::string& appname,
                  const std::filesystem::path& logPath) {
    appName = appname;
    std::filesystem::path logFolder = GetLogFolderPath();
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
    if (std::filesystem::exists(fullLogPath)) {
        try {
            std::filesystem::rename(fullLogPath, prevLogPath);
        } catch (const std::filesystem::filesystem_error& e) {
            SDL_Log("Failed to rename previous log file: %s", e.what());
        }
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
        Logger::Log("Logger shutting down.");
        logFile->flush();
        logFile->close();
        logFile = nullptr;
    }
}

void Logger::Log(const std::string& message, LogLevel level) {
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
}

void Logger::LogHardwareInfo(Syngine::Core& core) {
    if (!logFile || !logFile->is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Log file is not open.");
        return;
    }

    Logger::Log("System Specifications:\n" + core.GetSystemSpecifications(), LogLevel::INFO);
}

} // namespace Syngine
