// ╒════════════════════════ FsUtils.cpp ═╕
// │ Syngine                              │
// │ Created 2025-07-18                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "SDL3/SDL_log.h"
#include "Syngine/Core/Logger.h"
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

#include "FsUtils.h"

#include "miniscl.hpp"

namespace Syngine {

std::filesystem::path _GetAppDataPath(const std::string& appName) {
    std::filesystem::path appDataFolder;
#ifdef _WIN32
    // On Windows, use the user's Roaming AppData folder.
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path);
    if (SUCCEEDED(hr)) {
        // Compose the Appdata folder path: AppData\Roaming\SentyTek\<appName>
        appDataFolder = std::filesystem::path(path) / "SentyTek" / appName;
        CoTaskMemFree(path);
    } else {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to get AppData path.");
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
    if (FSRefMakePath(&ref, (UInt8*)path, sizeof(path)) != noErr) {
        std::cerr << "Error converting FSRef to path." << std::endl;
        return "";
    }

    // Compose the Appdata folder path: ~/Library/Application Support/SentyTek/<appName>
    appDataFolder = std::filesystem::path(path) / "SentyTek" / appName;

#else
    // On Linux and other platforms, use XDG_DATA_HOME.
    const char* homeDir = std::getenv("XDG_DATA_HOME");
    std::string homePath;

    if (!homeDir) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                     "XDG_DATA_HOME not set for AppData path.");

        // Fallback to manual home directory resolution
        homeDir = std::getenv("HOME");
        if (!homeDir) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                "HOME environment variable not set for AppData path.");
        }

        homePath = std::string(homeDir) + "/.local/share"; // Default fallback to ~/.local/share
    }

    // Compose the AppData folder path: $XDG_DATA_HOME/SentyTek/<appName> (or ~/.local/share/SentyTek/<appName>)
    appDataFolder = std::filesystem::path(homePath) / "SentyTek" / appName;
    SDL_Log("%s", appDataFolder.string().c_str());

#endif
    // Log an error if the path could not be determined.
    if (appDataFolder.empty()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to determine AppData folder path.");
    }
    return appDataFolder;
}

bool _FileExists(const char* path) {
    std::string fullPath = Syngine::_ResolveOSPath(path);
    auto        p        = scl::path(fullPath);
    return p.exists();
}

} // namespace Syngine
