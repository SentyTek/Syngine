// ╒═══════════════════════ LuaManager.h ═╕
// │ Syngine                              │
// │ Created 2026-04-13                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once

#include "sol/forward.hpp"
#include <Syngine/Core/Logger.h>
#include <Syngine/ECS/GameObject.h>

namespace sol {
class state; // forward declaration
struct variadic_args;
}

namespace Syngine {

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

/// @brief Manages the Lua state and scripting environment for the engine.
/// @section Lua
class LuaManager {
    static sol::state* m_luaState;
    static sol::state* _GetState() { return m_luaState; }
    static bool        m_initialized;
    static bool        m_allowTicking; // Whether to allow ticking Lua scripts (set to false during reloads)

    static void _RegisterEntityBindings(sol::state& lua);
    static void _RegisterInputBindings(sol::state& lua);

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

    static void _ReloadLuaState();

    static std::vector<GameObject*>
        m_ownedObjects; // List of GameObjects owned by Lua (for cleanup)

    static LuaManager* m_instance; // Singleton instance of LuaManager

    static LuaLibs m_libs; // Libraries to open in the Lua state

    friend struct ComponentRegistrar; // Allow ComponentRegistrar to register Lua
                                     // bindings
    friend class Core;

    // Template specializations for common signatures
    template <typename Func>
    static void _AddFunctionImpl(const std::string& name,
                                 Func               func,
                                 const std::string& namespace_);

    // void() - no args
    template <>
    void _AddFunctionImpl<std::function<void()>>(const std::string&    name,
                                                 std::function<void()> func,
                                                 const std::string& namespace_);

    // void(bool)
    template <>
    void
    _AddFunctionImpl<std::function<void(bool)>>(const std::string&        name,
                                                std::function<void(bool)> func,
                                                const std::string& namespace_);

    // void(int)
    template <>
    void
    _AddFunctionImpl<std::function<void(int)>>(const std::string&       name,
                                               std::function<void(int)> func,
                                               const std::string& namespace_);
  public:
    friend LuaLibs operator|(LuaLibs a, LuaLibs b) {
        return static_cast<LuaLibs>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    friend LuaLibs operator&(LuaLibs a, LuaLibs b) {
        return static_cast<LuaLibs>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
    friend LuaLibs operator~(LuaLibs a) {
        return static_cast<LuaLibs>(~static_cast<uint32_t>(a));
    }

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

    /// @brief Add a function to the Lua state with the given name and category
    /// @param name The name of the function to add
    /// @tparam Func The function type (e.g., std::function<void(int)>)
    /// @param namespace_ The namespace to add the function under (e.g., "game",
    /// "input", etc.)
    template <typename Func>
    static void AddFunction(const std::string& name,
                            Func               func,
                            const std::string& namespace_ = "") {
        if (!m_luaState) {
            Logger::Error("Lua state is not initialized. Cannot add function.");
            return;
        }
        _AddFunctionImpl(name, func, namespace_);
    }

    /// @brief Check if the Lua state has an object with the given name
    /// @param name The name of the object to check for
    /// @return True if the object exists in the Lua state, false otherwise
    static bool HasObject(const std::string& name);

    /// @brief Does a function if it exists in the Lua state, with the given
    /// arguments
    /// @param funcName The name of the function to call
    /// @param data Pointer to the data to pass as arguments
    /// @param dataSize Size of the data in bytes
    static void
    DoFunction(const std::string& funcName, const void* data, size_t dataSize);

    /// @brief Ticks the Lua state by calling the "onTick" function if it
    /// exists, with the given delta time
    /// @param physDeltaTime The time elapsed since the last physics tick, in seconds
    /// @param realDeltaTime The real time elapsed since the last tick, in seconds (not affected by time scaling)
    /// @param simulating Whether the simulation is currently running
    static void DoTick(float physDeltaTime, float realDeltaTime, bool simulating);

    /// @brief Set whether to allow ticking Lua scripts (used to prevent ticking
    /// during reloads)
    /// @param allow True to allow ticking, false to prevent it
    /// @return The previous allow ticking state
    static inline bool SetAllowTicking(bool allow) {
        bool previous = m_allowTicking;
        m_allowTicking = allow;
        return previous;
    }
};

} // namespace Syngine
