// ╒═════════════════════ LuaManager.cpp ═╕
// │ Syngine                              │
// │ Created 2026-04-13                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Core/LuaManager.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/ECS/AllComponents.h"
#include "Syngine/ECS/ComponentRegistry.h"
#include "Syngine/Utils/FsUtils.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace Syngine {

namespace {

std::string _ToLowerCopy(std::string value) {
    std::transform(value.begin(),
                   value.end(),
                   value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string _NormalizeComponentType(const std::string& rawType) {
    const std::string lowered = _ToLowerCopy(rawType);

    if (lowered == "transform" || lowered == "transformcomponent") {
        return "TransformComponent";
    }
    if (lowered == "mesh" || lowered == "meshcomponent") {
        return "MeshComponent";
    }
    if (lowered == "rigidbody" || lowered == "rigidbodycomponent") {
        return "RigidbodyComponent";
    }
    if (lowered == "billboard" || lowered == "billboardcomponent") {
        return "BillboardComponent";
    }

    return rawType;
}

PhysicsShapes _ParsePhysicsShape(const sol::object& shapeObj,
                                 PhysicsShapes      fallback) {
    if (!shapeObj.valid() || shapeObj == sol::lua_nil) {
        return fallback;
    }

    if (shapeObj.is<int>()) {
        return static_cast<PhysicsShapes>(shapeObj.as<int>());
    }

    if (shapeObj.is<std::string>()) {
        const std::string shape = _ToLowerCopy(shapeObj.as<std::string>());
        static const std::unordered_map<std::string, PhysicsShapes> kShapeMap = {
            { "sphere", PhysicsShapes::SPHERE },
            { "box", PhysicsShapes::BOX },
            { "capsule", PhysicsShapes::CAPSULE },
            { "capsule_tapered", PhysicsShapes::CAPSULE_TAPERED },
            { "cylinder", PhysicsShapes::CYLINDER },
            { "cylinder_tapered", PhysicsShapes::CYLINDER_TAPERED },
            { "cone", PhysicsShapes::CONE },
            { "convex_hull", PhysicsShapes::CONVEX_HULL },
            { "plane", PhysicsShapes::PLANE },
            { "mesh", PhysicsShapes::MESH },
            { "compound", PhysicsShapes::COMPOUND },
        };

        auto it = kShapeMap.find(shape);
        if (it != kShapeMap.end()) {
            return it->second;
        }
    }

    return fallback;
}

RigidbodyParameters _ParseRigidbodyParams(sol::optional<sol::table> maybeParams) {
    RigidbodyParameters params{};
    if (!maybeParams) {
        return params;
    }

    const sol::table paramTable = *maybeParams;

    params.mass = paramTable.get_or("mass", params.mass);
    params.friction = paramTable.get_or("friction", params.friction);
    params.restitution = paramTable.get_or("restitution", params.restitution);

    const sol::object shapeObj = paramTable["shape"];
    params.shape = _ParsePhysicsShape(shapeObj, params.shape);

    const sol::object shapeParamsObj = paramTable["shapeParameters"];
    if (shapeParamsObj.valid() && shapeParamsObj.get_type() == sol::type::table) {
        params.shapeParameters.clear();
        sol::table shapeParamsTable = shapeParamsObj.as<sol::table>();
        for (size_t i = 1;; ++i) {
            sol::optional<float> maybeValue = shapeParamsTable[i];
            if (!maybeValue) {
                break;
            }
            params.shapeParameters.push_back(*maybeValue);
        }
    }

    const sol::object motionTypeObj = paramTable["motionType"];
    if (motionTypeObj.valid() && motionTypeObj.is<int>()) {
        params.motionType = static_cast<JPH::EMotionType>(motionTypeObj.as<int>());
    }

    const sol::object layerObj = paramTable["layer"];
    if (layerObj.valid() && layerObj.is<int>()) {
        params.layer = static_cast<JPH::ObjectLayer>(layerObj.as<int>());
    }

    return params;
}

} // namespace

// Static member definitions
sol::state* LuaManager::m_luaState = nullptr;
bool        LuaManager::m_initialized = false;

// Simply links into the logger to print info.
sol::object _SynginePrint(sol::variadic_args va) {
    std::string output = "[Lua/init.lua] ";
    for (size_t i = 0; i < va.size(); ++i) {
        sol::object obj = va[i];
        if (obj.is<std::string>()) {
            output += obj.as<std::string>();
        } else if (obj.is<int>()) {
            output += std::to_string(obj.as<int>());
        } else if (obj.is<double>()) {
            output += std::to_string(obj.as<double>());
        } else if (obj.is<bool>()) {
            output += obj.as<bool>() ? "true" : "false";
        } else {
            output += "<non-printable type>";
        }
        if (i < va.size() - 1) {
            output += "\t";
        }
    }

    Logger::LogF(LogLevel::INFO, "%s", output.c_str());
    return sol::lua_nil;
}

void LuaManager::_RegisterEntityBindings(sol::state& lua) {
    // Register GameObject as a usertype
    auto gameObjectType = lua.new_usertype<Syngine::GameObject>("GameObject");
    gameObjectType["name"] = &Syngine::GameObject::name;
    gameObjectType["type"] = &Syngine::GameObject::type;
    gameObjectType["enabled"] = &Syngine::GameObject::enabled;
    gameObjectType["AddComponent"] = [&lua](sol::this_state,
                                             Syngine::GameObject* obj,
                                             std::string          type,
                                             sol::variadic_args   args) -> sol::object {
        const std::string normalizedType = _NormalizeComponentType(type);

        if (normalizedType == "TransformComponent") {
            TransformComponent* comp = obj->AddComponent<TransformComponent>();
            if (!comp) {
                Logger::LogF(LogLevel::ERR,
                             "Failed to add component '%s' in Lua (already exists?)",
                             type.c_str());
                return sol::lua_nil;
            }
            return sol::make_object(lua, comp);
        } else if (normalizedType == "MeshComponent") {
            MeshComponent* comp = nullptr;

            // No args
            if (args.size() == 0) {
                Logger::Warn(
                    "Adding MeshComponent with no arguments in Lua is not "
                    "recommended since it won't have a mesh assigned. Consider "
                    "providing at least a path to a model.");
                return sol::lua_nil;
            } else if (args.size() == 2) {
                sol::object arg0 = args[0];
                sol::object arg1 = args[1];

                // bundle, path
                if (arg0.is<std::string>() && arg1.is<std::string>()) {
                    comp = obj->AddComponent<MeshComponent>(arg0.as<std::string>(), arg1.as<std::string>());
                }
            } else {
                // Try to parse as (bundle, path, hasTextures)
                sol::object arg0 = args[0];
                sol::object arg1 = args[1];
                sol::object arg2 = args[2];

                if (arg0.is<std::string>() && arg1.is<std::string>() && arg2.is<bool>()) {
                    comp = obj->AddComponent<MeshComponent>(arg0.as<std::string>(), arg1.as<std::string>(), arg2.as<bool>());
                } else {
                    Logger::LogF(LogLevel::ERR,
                                 "Invalid arguments for adding MeshComponent in Lua. Expected (bundle, path) or (bundle, path, hasTextures).");
                    return sol::lua_nil;
                }
            }

            if (!comp) {
                Logger::LogF(LogLevel::ERR,
                             "Failed to add MeshComponent in Lua (bad args or already exists?)",
                             type.c_str());
                return sol::lua_nil;
            }
            return sol::make_object(lua, comp);
        } else if (normalizedType == "RigidbodyComponent") {
            sol::optional<sol::table> params = sol::nullopt;
            if (args.size() >= 1) {
                sol::object arg0 = args[0];
                if (arg0.get_type() == sol::type::table) {
                    params = arg0.as<sol::table>();
                }
            }

            const RigidbodyParameters rbParams = _ParseRigidbodyParams(params);
            RigidbodyComponent* comp = obj->AddComponent<RigidbodyComponent>(rbParams);
            if (!comp) {
                Logger::LogF(LogLevel::ERR,
                             "Failed to add component '%s' in Lua (already exists?)",
                             type.c_str());
                return sol::lua_nil;
            }
            return sol::make_object(lua, comp);
        } else if (normalizedType == "BillboardComponent") {
            if (args.size() < 2) {
                Logger::LogF(LogLevel::ERR,
                             "Invalid arguments for adding BillboardComponent in Lua. Expected (bundlePath, imagePath[, mode[, size]]).");
                return sol::lua_nil;
            }

            std::string bundlePath;
            std::string texturePath;
            BillboardMode mode = BillboardMode::CAMERA_ALIGNED;
            float size = 1.0f;

            // Parse args in order: (bundlePath, imagePath[, mode[, size]])
            if (args.size() >= 1 && args[0].is<std::string>()) {
                bundlePath = args[0].as<std::string>();
            }
            if (args.size() >= 2 && args[1].is<std::string>()) {
                texturePath = args[1].as<std::string>();
            } else {
                Logger::LogF(LogLevel::ERR,
                             "Invalid arguments for adding BillboardComponent in Lua. imagePath must be a string.");
                return sol::lua_nil;
            }
            if (args.size() >= 3 && args[2].is<std::string>()) {
                std::string modeStr = args[2].as<std::string>();
                if (modeStr == "CAMERA_ALIGNED") {
                    mode = BillboardMode::CAMERA_ALIGNED;
                } else if (modeStr == "Y_ALIGNED") {
                    mode = BillboardMode::AXIS_Y_ALIGNED;
                } else if (modeStr == "FIXED") {
                    mode = BillboardMode::FIXED;
                } else {
                    Logger::LogF(LogLevel::ERR,
                                 "Invalid billboard mode '%s' specified in Lua. Defaulting to CAMERA_ALIGNED.",
                                 modeStr.c_str());
                }
            }
            if (args.size() >= 4 && args[3].is<double>()) {
                size = static_cast<float>(args[3].as<double>());
            }

            BillboardComponent* comp = obj->AddComponent<BillboardComponent>(bundlePath, texturePath, mode, size);
            if (!comp) {
                Logger::LogF(LogLevel::ERR,
                             "Failed to add component '%s' in Lua (already exists?)",
                             type.c_str());
                return sol::lua_nil;
            }
            return sol::make_object(lua, comp);
        } else {
            Logger::LogF(LogLevel::ERR,
                         "Unknown component type '%s' requested in Lua",
                         type.c_str());
            return sol::lua_nil;
        }
    };
    gameObjectType["GetComponent"] = [&lua](Syngine::GameObject* obj,
                                             std::string          type) -> sol::object {
        const std::string normalizedType = _NormalizeComponentType(type);

        if (normalizedType == "TransformComponent") {
            TransformComponent* comp = obj->GetComponent<TransformComponent>();
            if (comp) return sol::make_object(lua, comp);
            return sol::lua_nil;
        } else if (normalizedType == "MeshComponent") {
            MeshComponent* comp = obj->GetComponent<MeshComponent>();
            if (comp) return sol::make_object(lua, comp);
            return sol::lua_nil;
        } else if (normalizedType == "RigidbodyComponent") {
            RigidbodyComponent* comp = obj->GetComponent<RigidbodyComponent>();
            if (comp) return sol::make_object(lua, comp);
            return sol::lua_nil;
        } else if (normalizedType == "BillboardComponent") {
            BillboardComponent* comp = obj->GetComponent<BillboardComponent>();
            if (comp) return sol::make_object(lua, comp);
            return sol::lua_nil;
        } else {
            Logger::LogF(LogLevel::ERR,
                         "Unknown component type '%s' requested in Lua",
                         type.c_str());
            return sol::lua_nil;
        }
    };

    // Create the scene namespace as a table
    sol::table scene = lua.create_table();
    scene["createGameObject"] = [&lua](std::string name,
                                         sol::optional<std::string> shader,
                                         sol::optional<std::string> tag) -> sol::object {
        // Create a new GameObject
        GameObject* go = new GameObject(name,
                                        shader.value_or("default"),
                                        tag.value_or(""));
        return sol::make_object(lua, go);
    };
    lua["scene"] = scene;
}

sol::object _AddComponent(sol::state_view lua, GameObject* obj, std::string type) {
    // For simplicity, we only support TransformComponent for now. In a real
    // implementation, you would have a factory or registry to create components
    // based on the type string.
    if (type == "TransformComponent") {
        TransformComponent* comp = obj->AddComponent<TransformComponent>();
        return sol::make_object(lua, comp);
    }

    Logger::LogF(LogLevel::ERR,
                 "Unknown component type '%s' requested in Lua",
                 type.c_str());
    return sol::lua_nil;
}

// Custom require function that looks for Lua scripts in the "scripts" directory
sol::object _CustomRequire(sol::this_state ts, const std::string& moduleName) {
    sol::state_view lua(ts);
    if (moduleName.empty()) {
        return sol::lua_nil;
    }

    std::string fullModuleName =  _GetAppDataPath("scripts/" + moduleName + ".lua").string();
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

void LuaManager::_RemoveBaseFuncs(sol::state& lua,
                                  bool        removeBase,
                                  bool        removeError,
                                  bool        removeMetatables,
                                  bool        removeIo,
                                  bool        removeOs,
                                  bool        removeRaw,
                                  bool        removeCoroutine,
                                  bool        removeDebug,
                                  bool        noSyngine) {
    // Remove potentially dangerous functions from the base library
    sol::table base = lua["base"];
    if (base.valid()) {
        if (removeBase) {
            base["dofile"] = nullptr;
            base["loadfile"] = nullptr;
            base["loadstring"] = nullptr;
            base["collectgarbage"] = nullptr;
            base["require"]        = _CustomRequire;
            base["package"]        = nullptr;
            base["load"]           = nullptr;
            base["module"]         = nullptr;
        }
        if (removeDebug) {
            lua["debug"] = nullptr;
            base["print"] = nullptr;
        }
        if (removeError) {
            base["error"] = nullptr;
            base["xpcall"] = nullptr;
            base["pcall"]  = nullptr;
            base["assert"] = nullptr;
            base["warn"]   = nullptr;
        }
        if (removeIo) {
            lua["io"] = nullptr;
        }
        if (removeOs) {
            lua["os"] = nullptr;
        }
        if (removeRaw) {
            base["rawset"]         = nullptr;
            base["rawget"]         = nullptr;
            base["rawequal"]       = nullptr;
            base["rawlen"]         = nullptr;
        }
        if (removeMetatables) {
            // Optionally remove metatable functions for extra security
            base["setmetatable"] = nullptr;
            base["getmetatable"] = nullptr;
        }

        // Add in our own
        if (!noSyngine) {
            lua["syngine"] = lua.create_table_with("log", _SynginePrint);
            _RegisterEntityBindings(lua);
            ComponentRegistry::_RegisterAllLuaBindings(lua);
        }
    }
}

LuaManager::LuaManager(LuaLibs args) {
    m_initialized = false;
    m_luaState = new sol::state();

    if (!m_luaState) {
        Logger::Error("Failed to create Lua state. Check the log for more details.");
        return;
    }

    // Lambda to help figure out which libraries to close
    auto has = [args](LuaLibs f) {
        return (static_cast<uint32_t>(args) & static_cast<uint32_t>(f)) != 0;
    };

    // Open libraries based on the specified flags
    m_luaState->open_libraries(sol::lib::base,
                               sol::lib::string,
                               sol::lib::utf8,
                               sol::lib::math,
                               sol::lib::table,
                               sol::lib::package,
                               sol::lib::coroutine,
                               sol::lib::debug,
                               sol::lib::io,
                               sol::lib::os);

    _RemoveBaseFuncs(*m_luaState,
                     !has(LuaLibs::DEFAULT),
                     !has(LuaLibs::ERRHAND),
                     has(LuaLibs::NOMETATABLES),
                     !has(LuaLibs::IO),
                     !has(LuaLibs::OS),
                     !has(LuaLibs::RAWFAMILY),
                     !has(LuaLibs::COROUTINES),
                     !has(LuaLibs::DEBUG),
                     has(LuaLibs::NOSYNGINE));

    m_initialized = true;
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

void LuaManager::SafeFile(const std::string& filePath) {
    if (!m_luaState) {
        Logger::Error("Lua state is not initialized. Cannot execute file.");
        return;
    }

    const sol::protected_function_result result =
        m_luaState->safe_script_file(filePath, sol::script_pass_on_error);

    if (!result.valid()) {
        const sol::error err = result;
        Logger::LogF(LogLevel::ERR,
                     "Lua error during file execution '%s': %s",
                     filePath.c_str(),
                     err.what());
    }
}

template <typename Func>
void LuaManager::AddFunction(const std::string& name, Func func, const std::string& table) {
    if (!m_luaState) {
        Logger::Error("Lua state is not initialized. Cannot add function.");
        return;
    }

    if (table.empty()) {
        (*m_luaState)[name] = func;
    } else {
        sol::table tbl = (*m_luaState)[table];
        if (!tbl.valid()) {
            tbl = (*m_luaState)[table] = m_luaState->create_table();
        }
        tbl[name] = func;
    }
}

} // namespace Syngine
