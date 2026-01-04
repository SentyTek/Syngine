// ╒═════════════════════════ Logger.cpp ═╕
// │ Syngine                              │
// │ Created 2025-07-14                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#elif defined(__linux__) || defined(__APPLE__)
#include <execinfo.h>
#include <unistd.h>
#include <cxxabi.h>
#endif

#include "Syngine/Core/Core.h"
#include "Syngine/Core/Logger.h"
#include "Syngine/Utils/FsUtils.h"

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

#ifdef _WIN32
#include <intrin.h>
#define DEBUG_BREAK() __debugbreak()
#elif __APPLE__
#include <csignal>
#define DEBUG_BREAK() raise(SIGABRT)
#else
#define DEBUG_BREAK() __builtin_trap()
#endif

namespace Syngine {

void Logger::SetupCrashHandler() {
    // Setup common signal handlers
    signal(SIGSEGV, CrashHandler);
    signal(SIGABRT, CrashHandler);
    signal(SIGFPE, CrashHandler);
    signal(SIGILL, CrashHandler);
    signal(SIGTERM, CrashHandler);

#ifdef _WIN32
    // Setup Windows exception handler
    SetUnhandledExceptionFilter(WindowsExceptionHandler);

    SymInitialize(GetCurrentProcess(), NULL, TRUE);
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
#endif

    Logger::Info("Crash handler initialized");
}

void Logger::CrashHandler(int signal) {
    const char* signalName = "Unknown";
    switch (signal) {
        case SIGSEGV: signalName = "SIGSEGV (Segmentation Fault)"; break;
        case SIGABRT: signalName = "SIGABRT (Abort)"; break;
        case SIGFPE:  signalName = "SIGFPE (Floating Point Exception)";  break;
        case SIGILL:  signalName = "SIGILL (Illegal Instruction)";  break;
        case SIGTERM: signalName = "SIGTERM (Termination)"; break;
        default:      signalName = "Unknown (Signal)"; break;
    }

    Logger::Log("=== CRASH DETECTED ===", LogLevel::ERR);
    Logger::Log("Signal: " + std::string(signalName), LogLevel::ERR);
    Logger::Log("Generating stack trace...", LogLevel::ERR);

    PrintStackTrace();

    Logger::Log("=== END OF CRASH REPORT ===", LogLevel::ERR);
    Logger::Log(appName + " has crashed. Press OK to exit. This should be "
                          "reported to the developers.",
                LogLevel::FATAL);
}

#ifdef _WIN32
LONG WINAPI Logger::WindowsExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo) {
    Logger::Log("=== CRASH DETECTED ===", LogLevel::ERR);
    Logger::Log("Exception Code: " + std::to_string(ExceptionInfo->ExceptionRecord->ExceptionCode), LogLevel::ERR);
    Logger::Log("Generating stack trace...", LogLevel::ERR);

    PrintStackTrace();

    Logger::Log("=== END OF CRASH REPORT ===", LogLevel::ERR);
    Logger::Log(appName + " has crashed. Press OK to exit. This should be "
                          "reported to the developers.",
                LogLevel::FATAL);

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void Logger::PrintStackTrace() {
#ifdef _WIN32
    // Windows stack trace provided by dbghelp
    HANDLE process = GetCurrentProcess();
    HANDLE thread  = GetCurrentThread();

    CONTEXT context;
    memset(&context, 0, sizeof(CONTEXT));
    context.ContextFlags = CONTEXT_FULL;
    RtlCaptureContext(&context);

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));

    DWORD imageType;
#ifdef _M_IX86
    imageType = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset = context.Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
    imageType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = context.Rsp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#endif

    char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    PSYMBOL_INFO symbol = (PSYMBOL_INFO)symbolBuffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen   = MAX_SYM_NAME;

    IMAGEHLP_LINE64 line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    int frameNum = 0;
    while (StackWalk64(imageType,
                       process,
                       thread,
                       &stackFrame,
                       &context,
                       NULL,
                       SymFunctionTableAccess64,
                       SymGetModuleBase64,
                       NULL)) {
        if (frameNum > 50) break; // Prevent infinite loops

        DWORD64 address = stackFrame.AddrPC.Offset;
        char    addrStr[32];
        sprintf(addrStr, "0x%016llX", address);
        std::string frameInfo = "Frame " + std::to_string(frameNum) + ": " + addrStr;

        if (SymFromAddr(process, address, NULL, symbol)) {
            frameInfo += " - " + std::string(symbol->Name);

            DWORD displacement;
            if (SymGetLineFromAddr64(process, address, &displacement, &line)) {
                frameInfo += " (" + std::string(line.FileName) + ":" +
                             std::to_string(line.LineNumber) + ")";
            }
        }

        Logger::Log(frameInfo, LogLevel::ERR);
        frameNum++;
    }
#elif defined(__linux__) || defined(__APPLE__)
    // Linux/Mac stack trace using backtrace()
    void* array[50];
    size_t size = backtrace(array, 50);
    char** symbols = backtrace_symbols(array, size);

    if (symbols == NULL) {
        Logger::Log("Failed to obtain stack trace symbols.", LogLevel::ERR);
        return;
    }

    for (size_t i = 0; i < size; i++) {
        std::string frameInfo = "Frame " + std::to_string(i) + ": ";

        // Try to demangle C++ symbols
        char* demangled = nullptr;
        char* mangledStart = strchr(symbols[i], '(');
        char* mangledEnd   = strchr(symbols[i], '+');

        if (mangledStart && mangledEnd && mangledEnd > mangledStart) {
            *mangledEnd = '\0';
            int status;
            demangled = abi::__cxa_demangle(mangledStart + 1, 0, 0, &status);
            if (status == 0 && demangled) {
                frameInfo += std::string(demangled);
            } else {
                frameInfo += std::string(mangledStart + 1);
            }
            free(demangled);
            *mangledEnd = '+';
        } else {
            frameInfo += symbols[i];
        }

        Logger::Log(frameInfo, LogLevel::ERR);
    }

    free(symbols);
#else
    Logger::Log("Stack trace not supported on this platform.", LogLevel::ERR);
#endif
}

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
        case LogLevel::ERR:   return "ERRR";
        case LogLevel::FATAL: return "FATL";
        default:              return "UNKN";
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

    SetupCrashHandler();

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
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR, "Fatal Error", finalMessage.c_str(), nullptr);

        // Break if debug is on
        /*
            !!!
            DEVELOPER:
            LOOK AT STACK TRACE
            The program is already safed by this point, so you can inspect the state.
            When done, resume execution. This will terminate the program.
            !!!
        */
#ifdef _DEBUG
        DEBUG_BREAK();
#endif
        
        exit(EXIT_FAILURE);
    }
}

void Logger::LogHardwareInfo() {
    if (!logFile || !logFile->is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Log file is not open.");
        return;
    }

    // This is a bit messy but it avoids duplicating code in multiple places
    Syngine::HardwareSpecs specs = Syngine::Core::Get()->GetSystemSpecifications();
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

void Logger::InfoPopup(const std::string_view message) {
    Log(message, LogLevel::INFO);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Info", message.data(), nullptr);
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
