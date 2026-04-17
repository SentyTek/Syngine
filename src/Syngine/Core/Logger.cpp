// ╒═════════════════════════ Logger.cpp ═╕
// │ Syngine                              │
// │ Created 2025-07-14                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#elif defined(__linux__) || defined(__APPLE__)
#include <execinfo.h>
#include <unistd.h>
#include <csignal>
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
#include <cstring>
#include <string_view>
#include <vector>
#include <cstdlib>
#include <exception>

#ifdef _WIN32
#include <intrin.h>
#define DEBUG_BREAK() __debugbreak()
#elif __APPLE__
#define DEBUG_BREAK() raise(SIGABRT)
#else
#define DEBUG_BREAK() __builtin_trap()
#endif

namespace Syngine {

void Logger::SetVerbose(bool verbose) {
    m_verbose = verbose;
}

bool Logger::IsVerbose() {
    return m_verbose;
}
static std::string DescribeCurrentException() {
    try {
        throw; // Re-throw the current exception to capture its type and message
    } catch (const std::exception& e) {
        return std::string("std::exception: ") + e.what();
    } catch (...) {
        return "Unhandled non-std exception.";
    }
}

static void LoggerTerminateHandler() {
    // If terminate was triggered by an unhandled exception, try to log its
    // details
    std::string msg = "std::terminate called";
    if (std::current_exception()) {
        try {
            std::rethrow_exception(std::current_exception());
        } catch (...) {
            msg += " ";
            msg += DescribeCurrentException();
        }
    }

    Logger::Log("=== CRASH DETECTED ===", LogLevel::ERR);
    Logger::Log(msg, LogLevel::ERR);
    Logger::PrintStackTrace();
    Logger::Log("=== END OF CRASH REPORT ===", LogLevel::ERR);
    Logger::Flush();
    std::_Exit(EXIT_FAILURE);
}

#ifdef _WIN32
const char* SehCodeToString(DWORD code) {
    switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
        case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT";
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT";
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
        case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT";
        case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION";
        case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW";
        case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK";
        case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW";
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION";
        case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO";
        case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW";
        case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
        case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION";
        case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP";
        case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW";
        case EXCEPTION_GUARD_PAGE:               return "EXCEPTION_GUARD_PAGE";
        default:                                 return "UNKNOWN_SEH_EXCEPTION";
    }
}
#endif

void Logger::SetupCrashHandler() {
    // Setup common signal handlers for crashes
#if defined(__linux__) || defined(__APPLE__)
    struct sigaction signalAction {};
    signalAction.sa_handler = _CrashHandler;
    sigemptyset(&signalAction.sa_mask);
    signalAction.sa_flags = SA_RESTART;

    sigaction(SIGSEGV, &signalAction, nullptr);
    sigaction(SIGABRT, &signalAction, nullptr);
    sigaction(SIGFPE, &signalAction, nullptr);
    sigaction(SIGILL, &signalAction, nullptr);
    sigaction(SIGTERM, &signalAction, nullptr);
#else
    signal(SIGSEGV, _CrashHandler);
    signal(SIGABRT, _CrashHandler);
    signal(SIGFPE, _CrashHandler);
    signal(SIGILL, _CrashHandler);
    signal(SIGTERM, _CrashHandler);
#endif

    std::set_terminate(LoggerTerminateHandler);

#ifdef _WIN32
    // Setup Windows exception handler
    SetUnhandledExceptionFilter(_WindowsExceptionHandler);
    SymInitialize(GetCurrentProcess(), NULL, TRUE);
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
#endif

    Logger::Info("Crash handler initialized");
}

void Logger::_CrashHandler(int signal) {
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
    Logger::Log(m_appName + " has crashed. Press OK to exit. This should be "
                          "reported to the developers.",
                LogLevel::FATAL);
}

#ifdef _WIN32
LONG WINAPI Logger::_WindowsExceptionHandler(EXCEPTION_POINTERS* ep) {
    const DWORD code = ep->ExceptionRecord->ExceptionCode;
    Logger::Log("=== CRASH DETECTED ===", LogLevel::ERR);
    Logger::Log("Exception Code: " + std::to_string(code), LogLevel::ERR);
    Logger::Log("Exception Address: " + std::to_string((uintptr_t)ep->ExceptionRecord->ExceptionAddress), LogLevel::ERR);
    Logger::Log("Exception Type: " + std::string(SehCodeToString(code)), LogLevel::ERR);

    if (code == EXCEPTION_ACCESS_VIOLATION && ep->ExceptionRecord->NumberParameters >= 2) {
        const auto mode = ep->ExceptionRecord->ExceptionInformation[0]; // 0 read, 1 write, 8 execute
        const auto addr = ep->ExceptionRecord->ExceptionInformation[1];
        Logger::LogF(LogLevel::ERR, "Access violation: mode=%llu address=0x%llX",
                     (unsigned long long)mode, (unsigned long long)addr);
    }

    Logger::Log("Generating stack trace...", LogLevel::ERR);

    PrintStackTrace();

    Logger::Log("=== END OF CRASH REPORT ===", LogLevel::ERR);
    Logger::Log(m_appName + " has crashed. Press OK to exit. This should be "
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

std::string Logger::_GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char        buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now_c));
    return std::string(buf);
}

[[nodiscard]]
std::string Logger::_LogLevelToString(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERR:   return "ERRR";
        case LogLevel::FATAL: return "FATL";
        default:              return "UNKN";
    }
}

void Logger::_Init(const std::string&           appname,
                  const std::filesystem::path& logPath,
                  bool                         verbose) {
    m_appName = appname;
    std::filesystem::path logFolder = Syngine::_GetAppdataPath(m_appName) / "logs";
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
    m_logFile = std::make_unique<std::ofstream>(fullLogPath,
                                              std::ios::out | std::ios::trunc);
    if (!m_logFile->is_open()) {
        // Log file probably doen't exist, try to create it
        std::ofstream testFile(fullLogPath);
        if (!testFile.is_open()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create log file at %s", fullLogPath.string().c_str());
            m_logFile = nullptr;
            return;
        }
        testFile.close();

        // Try opening again
        m_logFile = std::make_unique<std::ofstream>(
            fullLogPath, std::ios::out | std::ios::trunc);
        if (!m_logFile->is_open()) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open log file at %s", fullLogPath.string().c_str());
            m_logFile = nullptr;
            return;
        }
    }

    SetupCrashHandler();

    Logger::Info("Logger initialized for " + m_appName, false);
    Logger::Log("Log file located at: " + fullLogPath.string());
    Logger::Log("Previous log (if any) moved to: " + prevLogPath.string());

    return;
}

void Logger::_Shutdown() {
    if (m_logFile && m_logFile->is_open()) {
        // The mutex is already locked if called from Log(FATAL),
        // so we write directly to the file instead of calling Log().
        (*m_logFile) << "[" << _GetTimestamp() << "] "
                   << "[" << _LogLevelToString(LogLevel::INFO) << "] "
                   << "Logger shutting down" << std::endl;
        m_logFile->flush();
        m_logFile->close();
        m_logFile = nullptr;
    }
}

void Logger::Log(const std::string_view message,
                 LogLevel               level,
                 bool                   writeOnlyInDebug,
                 bool                   toConsole) {
    if (!m_logFile || !m_logFile->is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Attempted to log while file is not open. Message: %s", message.data());
        return;
    }

    if (level < m_minLogLevel) {
        return; // Skip logging if level is below current threshold
    }

    std::lock_guard<std::mutex> lock(m_logMutex);

#ifdef NDEBUG
    // In release mode, skip messages that are marked as writeOnlyInDebug
    // Unless the --verbose flag is set, then write them anyway
    try {
        if (m_verbose || !writeOnlyInDebug) {
            (*m_logFile) << "[" << _GetTimestamp() << "] "
                    << "[" << _LogLevelToString(level) << "] "
                    << message << std::endl;
            if (m_autoFlush) {
                m_logFile->flush();
            }
        }
    } catch (const std::exception& e) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to log message: %s", e.what());
    }
#else
    // Always write in debug mode
    try {
        (*m_logFile) << "[" << _GetTimestamp() << "] "
                << "[" << _LogLevelToString(level) << "] "
                << message << std::endl;
        if (m_autoFlush) {
            m_logFile->flush();
        }
    } catch (const std::exception& e) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to log message: %s", e.what());
    }
#endif

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
        _Shutdown();
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION, "Fatal error: %s", message.data());
        std::string finalMessage = m_appName + " has encountered a fatal error and needs to close:\n\n" + message.data();
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
    if (!m_logFile || !m_logFile->is_open()) {
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

    Logger::Log(specsStr, LogLevel::INFO, false);
}

void Logger::LogF(LogLevel level, bool writeOnlyInDebug, const char* fmt, ...) {
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

    Log(std::string(buffer.data()), level, writeOnlyInDebug);
}

void Logger::InfoPopup(const std::string_view message) {
    Log(message, LogLevel::INFO, false);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Info", message.data(), nullptr);
}

void Logger::Error(const std::string_view message, bool writeOnlyInDebug) { Log(message, LogLevel::ERR, writeOnlyInDebug); }
void Logger::Info(const std::string_view message, bool writeOnlyInDebug) { Log(message, LogLevel::INFO, writeOnlyInDebug); }
void Logger::Warn(const std::string_view message, bool writeOnlyInDebug) { Log(message, LogLevel::WARN, writeOnlyInDebug); }
void Logger::Fatal(const std::string_view message) { Log(message, LogLevel::FATAL); }

void Logger::Flush() {
    std::lock_guard<std::mutex> lock(m_logMutex);
    if (m_logFile && m_logFile->is_open()) {
        m_logFile->flush();
    }
}

} // namespace Syngine
