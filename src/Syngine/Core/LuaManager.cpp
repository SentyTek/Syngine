// ╒═════════════════════ LuaManager.cpp ═╕
// │ Syngine                              │
// │ Created 2026-04-13                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Core/LuaManager.h"
#include <sol/forward.hpp>
#include <sol/load_result.hpp>
#include <sol/optional_implementation.hpp>
#include <sol/state_view.hpp>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

namespace Syngine {

sol::object _CustomRequire(sol::this_state ts) {
    sol::state_view lua(ts);

    sol::optional<std::string> maybeName = lua["..."];
    if (!maybeName) {
        return sol::lua_nil;
    }

    std::string moduleName = *maybeName;
    if (moduleName.empty()) {
        return sol::lua_nil;
    }

    std::string fullModuleName = "scripts/" + moduleName + ".lua";
    Logger::LogF(
        LogLevel::INFO, "Requiring Lua module: %s", fullModuleName.c_str());

    sol::load_result lr = lua.load_file(fullModuleName);
    if (!lr.valid()) {
        sol::error err = lr;
        Logger::LogF(LogLevel::ERR,
                     "Error loading Lua module '%s': %s",
                     fullModuleName.c_str(),
                     err.what());
        return sol::lua_nil;
    }

    sol::protected_function_result res = lr();
    if (!res.valid()) {
        sol::error err = res;
        Logger::LogF(LogLevel::ERR,
                     "Error executing Lua module '%s': %s",
                     fullModuleName.c_str(),
                     err.what());
        return sol::lua_nil;
    }

    return res;
}

void _RemoveBaseFuncs(sol::state& lua, bool removeError = true, bool noMetatables = false) {
    // Remove potentially dangerous functions from the base library
    sol::table base = lua["base"];
    if (base.valid()) {
        base["dofile"] = nullptr;
        base["loadfile"] = nullptr;
        base["loadstring"] = nullptr;
        base["print"] = nullptr;
        if (removeError) {
            base["error"] = nullptr;
            base["xpcall"] = nullptr;
            base["pcall"]  = nullptr;
            base["assert"] = nullptr;
            base["warn"]   = nullptr;
        }
        base["collectgarbage"] = nullptr;
        base["rawset"]         = nullptr;
        base["rawget"]         = nullptr;
        base["rawequal"]       = nullptr;
        base["rawlen"]         = nullptr;
        base["require"]        = _CustomRequire;
        if (noMetatables) {
            // Optionally remove metatable functions for extra security
            base["setmetatable"] = nullptr;
            base["getmetatable"] = nullptr;
        }
    }
}

LuaManager::LuaManager(LuaLibs args) {
    m_luaState = new sol::state();

    if (!m_luaState) {
        Logger::Error("Failed to create Lua state. Check the log for more details.");
        return;
    }

    // Open libraries based on the specified flags
    if (args == LuaLibs::DEFAULT) {
        m_luaState->open_libraries(
            sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table);
        _RemoveBaseFuncs(*m_luaState);
    } else if (args == LuaLibs::ALL) {
        m_luaState->open_libraries(sol::lib::base,
                                   sol::lib::package,
                                   sol::lib::string,
                                   sol::lib::math,
                                   sol::lib::table,
                                   sol::lib::io,
                                   sol::lib::os,
                                   sol::lib::debug);
    } else if (args == LuaLibs::DEBUG) {
        m_luaState->open_libraries(sol::lib::debug);
        _RemoveBaseFuncs(*m_luaState);
    } else if (args == LuaLibs::IO) {
        m_luaState->open_libraries(sol::lib::io, sol::lib::base);
        _RemoveBaseFuncs(*m_luaState);
    } else if (args == LuaLibs::OS) {
        m_luaState->open_libraries(sol::lib::os, sol::lib::base);
        _RemoveBaseFuncs(*m_luaState);
    } else if (args == LuaLibs::ERRHAND) {
        m_luaState->open_libraries(sol::lib::base);
        _RemoveBaseFuncs(*m_luaState, false); // Keep error handling functions
    } else if (args == LuaLibs::NONE) {
        m_luaState->open_libraries(); // Open no libraries
    } else if (args == LuaLibs::NOMETATABLES) {
        m_luaState->open_libraries(sol::lib::base, sol::lib::string, sol::lib::math, sol::lib::table);
        _RemoveBaseFuncs(*m_luaState, true, true);
    } else {
        Logger::Error("Invalid LuaLibs flag specified. No libraries opened.");
    }
}

LuaManager::~LuaManager() {
    if (m_luaState) {
        delete m_luaState;
        m_luaState = nullptr;
    }
}

void LuaManager::SafeScript(const std::string& script) {
    if (!m_luaState) {
        Logger::Error("Lua state is not initialized. Cannot execute script.");
        return;
    }

    const sol::protected_function_result result =
        m_luaState->safe_script(script, sol::script_pass_on_error);

    if (!result.valid()) {
        const sol::error err = result;
        Logger::LogF(LogLevel::ERR,
                     "Lua error during script execution: %s",
                     err.what());
    }
}

} // namespace Syngine
