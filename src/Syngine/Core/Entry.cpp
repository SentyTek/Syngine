// ╒══════════════════════════ Entry.cpp ═╕
// │ Syngine                              │
// │ Created 2025-09-25                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

// Entry points for the program

#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#include <shellapi.h> // For CommandLineToArgvW
#include <cwchar>
#endif

#include <iostream>

#include "Syngine/Core/Logger.h"

// Forward declare the game's main function
extern int AppMain(int argc, char* argv[]);

#if defined(_WIN32)
void EnableConsole() {
    AllocConsole();

    // Redirect standard output to the console
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);
    std::ios::sync_with_stdio();
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR     lpCmd,
                   int       nShowCmd) {
    // Convert command line to argc/argv
    int argc;
    LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argvW) {
        return -1;
    }

    char** argv = new char*[argc];
    for (int i = 0; i < argc; ++i) {
        int len = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
        argv[i] = new char[len];
        WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argv[i], len, nullptr, nullptr);
    }

    for (int i = 0; i < argc; ++i) {
        if (wcscmp(argvW[i], L"--console") == 0) {
            EnableConsole();
        }
        if (wcscmp(argvW[i], L"--verbose") == 0) {
            Syngine::Logger::SetVerbose(true);
        }
    }

    // Call the main application function
    int result = 0;
    try {
        result = AppMain(argc, argv);
    } catch (const std::exception& e) {
        Syngine::Logger::Fatal(std::string("Unhandled exception in WinMain: ") + e.what());
        result = -1;
    } catch (...) {
        Syngine::Logger::Fatal("Unhandled non-std exception in WinMain");
        result = -1;
    }

    // Cleanup allocated memory
    for (int i = 0; i < argc; ++i) {
        delete[] argv[i];
    }
    delete[] argv;
    LocalFree(argvW);
    return result;
}
#else
int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--verbose") == 0) {
            Syngine::Logger::SetVerbose(true);
        }
    }

    int result = 0;
    try {
        result = AppMain(argc, argv);
    } catch (const std::exception& e) {
        Syngine::Logger::Fatal(std::string("Unhandled exception in main: ") + e.what());
        result = -1;
    } catch (...) {
        Syngine::Logger::Fatal("Unhandled non-std exception in main");
        result = -1;
    }
    return result;
}
#endif
