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
    static sol::state* m_luaState;
    static sol::state* _GetState() { return m_luaState; }
    static bool        m_initialized;

    static void _RegisterEntityBindings(sol::state& lua);
    static void _RemoveBaseFuncs(sol::state& lua,
                                 bool        removeBase       = true,
                                 bool        removeError      = true,
                                 bool        removeMetatables = false,
                                 bool        removeIo         = true,
                                 bool        removeOs         = true,
                                 bool        removeRaw        = true,
                                 bool        removeCoroutine  = true,
                                 bool        removeDebug      = true,
                                 bool        noSyngine        = false);

    friend class ComponentRegistrar; // Allow ComponentRegistrar to register Lua bindings
  public:
    /// @brief Enum for specifying which Lua libraries to open
    /// @since v0.0.1
    enum class LuaLibs : uint32_t {
        NONE = 0, //* Do not open any libraries
        DEFAULT =
            2, //* Open the default set of libraries (string, utf8, math, table,
               //* require)
        ALL   = 4, //* Open all standard Lua libraries
        DEBUG = 8, //* Open additional libraries useful for debugging (e.g.,
                   //* debug and print)
        IO = 16, //* Open the io library for file operations (use with caution)
        OS = 32, //* Open the os library for operating system interactions
        ERRHAND = 64, //* Open the default library for error handling and
                      //* stack traces (pcall, xpcall)
        NOMETATABLES =
            128, //* Do not set metatables on Lua objects (for extra security)
        COROUTINES = 256, //* Open the coroutines library for Lua coroutines
        RAWFAMILY  = 512, //* Open the raw* functions (rawget, rawset, etc.) for
                          //* low-level
                          //* table manipulation (use with caution)
        NOSYNGINE =
            1024, //* Do not register any Syngine bindings. This disables the
                  //* ECS, logger, and input bindings, and is intended for
                  //* sandboxed environments where you want to allow Lua scripting
                  //* but not give access to engine internals.
    };

    friend LuaLibs operator|(LuaLibs a, LuaLibs b) {
        return static_cast<LuaLibs>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    friend LuaLibs operator&(LuaLibs a, LuaLibs b) {
        return static_cast<LuaLibs>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
    friend LuaLibs operator~(LuaLibs a) {
        return static_cast<LuaLibs>(~static_cast<uint32_t>(a));
    }
    friend LuaLibs operator==(LuaLibs a, LuaLibs b) { return (a & b) == b; }
    friend LuaLibs operator!=(LuaLibs a, LuaLibs b) { return (a & b) != b; }

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
    static void SafeScript(const std::string& script);

    /// @brief Do a safe file execution with error handling
    /// @param filePath The path to the Lua file to execute
    static void SafeFile(const std::string& filePath);

    /// @brief Add a function to the Lua state
    /// @param name The name of the function in Lua
    /// @param func The C++ function to bind to Lua
    /// @param table Optional table name to add the function to (e.g., "syngine"). If empty, the function will be added to the global namespace.
    template <typename Func>
    static void AddFunction(const std::string& name, Func func, const std::string& table = "");
};
} // namespace Syngine
