// ╒═══════════════════════ LuaManager.h ═╕
// │ Syngine                              │
// │ Created 2026-04-13                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once

#include <Syngine/Core/Core.h>
#include <Syngine/Core/Logger.h>

namespace sol {
class state; // forward declaration
}

namespace Syngine {

/// @brief Manages the Lua state and scripting environment for the engine.
/// @section Lua
class LuaManager {
    sol::state* m_luaState;

  public:
    /// @brief Enum for specifying which Lua libraries to open
    /// @since v0.0.1
    enum class LuaLibs : uint32_t {
        NONE    = 0, //* Do not open any libraries
        DEFAULT = 2, //* Open the default set of libraries (string, math, table,
                     // require)
        ALL   = 4,   //* Open all standard Lua libraries
        DEBUG = 8,   //* Open additional libraries useful for debugging (e.g.,
                     // debug and print)
        IO = 16, //* Open the io library for file operations (use with caution)
        OS = 32, //* Open the os library for operating system interactions
        ERRHAND = 64, //* Open the default library for error handling and
                      // stack traces (pcall, xpcall)
        NOMETATABLES = 128, //* Do not set metatables on Lua objects (for extra security)
    };

    /// @brief Constructor that initializes the Lua state and opens libraries
    /// based on the specified flags
    /// @param args Variadic arguments to specify which libraries to open (see
    /// LuaLibs enum)
    /// @note By default, DEFAULT libraries are opened if no arguments are
    /// provided. Note that supplying OS, IO, ERRHAND, it will *also* open the
    /// DEFAULT libraries, as they are required for those to function properly.
    /// @since v0.0.1
    LuaManager(LuaLibs args = LuaLibs::DEFAULT);
    ~LuaManager();

    /// @brief Do a safe script execution with error handling
    /// @param script The Lua script to execute
    void SafeScript(const std::string& script);
};
} // namespace Syngine
