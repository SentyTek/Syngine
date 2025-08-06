// ╒═════════════════════════ Logger.cpp ═╕
// │ Syngine                              │
// │ Created 2025-07-14                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/Core.h"
#include "Syngine/Core/Logger.h"
#include "Syngine/Utils/FSUtils.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_messagebox.h"

#include <cstdio>
#include <fstream>
#include <chrono>
#include <ctime>
#include <cstdarg>
#include <string_view>
#include <vector>
#include <cstdlib>

namespace Syngine {

std::string Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char        buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    return std::string(buf);
}

[[nodiscard]]
std::string Logger::LogLevelToString(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERR:   return "ERR";
        case LogLevel::FATAL: return "FATAL";
        default:              return "UNKNOWN";
    }
}

void Logger::Init(const std::string& appname,
                  const std::filesystem::path& logPath) {
    appName = appname;
    std::filesystem::path logFolder = Syngine::_GetAppdataPath(appName) / "logs";
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

void Logger::Log(const std::string_view message, LogLevel level, bool toConsole) {
    if (!logFile || !logFile->is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Attempted to log while file is not open. Message: %s", message.data());
        return;
    }

    std::lock_guard<std::mutex> lock(logMutex);

    try {
        (*logFile) << "[" << GetTimestamp() << "] "
                << "[" << LogLevelToString(level) << "] "
                << message << std::endl;
        if (autoFlush) {
            logFile->flush();
        }
    } catch (const std::exception& e) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to log message: %s", e.what());
    }

    if (toConsole) {
        switch (level) {
            case LogLevel::INFO:
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "%s", message.data());
                break;
            case LogLevel::WARN:
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "%s", message.data());
                break;
            case LogLevel::ERR: 
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.data());
                break;
            case LogLevel::FATAL:
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.data());
                break;
            default:
                SDL_Log("%s", message.data());
                break;
        }
    }

    if (level == LogLevel::FATAL) {
        Shutdown();
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION, "Fatal error: %s", message.data());
        std::string finalMessage = appName + " has encountered a fatal error and needs to close:\n\n" + message.data();
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error", finalMessage.c_str(), nullptr);
        exit(EXIT_FAILURE);
    }
}

void Logger::LogHardwareInfo() {
    if (!logFile || !logFile->is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Log file is not open.");
        return;
    }

    // This is a bit messy but it avoids duplicating code in multiple places
    Syngine::HardwareSpecs specs = Syngine::Core::_Get()->GetSystemSpecifications();
    std::string            specsStr = "\nSystem Specifications:\n";
    specsStr += "\tOperating System: " + specs.osName + "\n";
    specsStr += "\tCPU: " + specs.cpuModel + "\n";
    specsStr += "\tCPU Architecture: " + specs.cpuArch + "\n";
    specsStr +=
        "\tNumber of processors: " + std::to_string(specs.cpuCores) + "\n";
    specsStr +=
        "\tTotal Physical Memory (MB): " + std::to_string(specs.ramMB) + "\n";
    
    specsStr += "\tDisplay Resolution: " + std::to_string(specs.screenWidth) +
                "x" + std::to_string(specs.screenHeight) + "\n";
    specsStr += "\tWindow Resolution: " + std::to_string(specs.winWidth) + "x" +
                std::to_string(specs.winHeight) + "\n";
    
    specsStr += "\tGPU Vendor ID: " + std::to_string(specs.gpuVendorID) + "\n";
    specsStr += "\tGPU Device ID: " + std::to_string(specs.gpuDeviceID) + "\n";

    specsStr +=
        "\tMax Texture Size: " + std::to_string(specs.maxTextureSize) + "\n";
    specsStr += "\tSupports Compute Shaders: " +
                std::string(specs.supportsCompute ? "Yes" : "No") + "\n";
    specsStr += "\tSupports 3D Textures: " +
                std::string(specs.supports3DTextures ? "Yes" : "No") + "\n";

    Logger::Log(specsStr, LogLevel::INFO);
}

void Logger::LogF(LogLevel level, const char* fmt, ...) {
    if (!fmt) return;

    va_list args;
    va_start(args, fmt);

    // Get required buffer data
    va_list argsCopy;
    va_copy(argsCopy, args);
    int len = vsnprintf(nullptr, 0, fmt, argsCopy);
    va_end(argsCopy);

    if (len < 0) {
        va_end(args);
        return;
    }

    // Allocate buffer and format the string
    std::vector<char> buffer(len + 1);
    vsnprintf(buffer.data(), buffer.size(), fmt, args);
    va_end(args);

    Log(std::string(buffer.data()), level);
}

void Logger::Error(const std::string_view message) { Log(message, LogLevel::ERR); }
void Logger::Info(const std::string_view message) { Log(message, LogLevel::INFO); }
void Logger::Warn(const std::string_view message) { Log(message, LogLevel::WARN); }
void Logger::Fatal(const std::string_view message) { Log(message, LogLevel::FATAL); }

void Logger::Flush() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile && logFile->is_open()) {
        logFile->flush();
    }
}

} // namespace Syngine
