// ╒═════════════════════ LuaManager.cpp ═╕
// │ Syngine                              │
// │ Created 2026-04-13                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Core/LuaManager.h"
#include "Syngine/Core/Input.h"
#include "Syngine/ECS/GameObject.h"
#include "Syngine/ECS/AllComponents.h"
#include "Syngine/ECS/ComponentRegistry.h"
#include "Syngine/Utils/FsUtils.h"
#include "Syngine/Core/Core.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <list>
#include <vector>
#include <string>

namespace Syngine {

// Various helper functions for Lua bindings
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
    if (lowered == "zone" || lowered == "zonecomponent") {
        return "ZoneComponent";
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
bool        LuaManager::m_allowTicking = true;
std::vector<GameObject*> LuaManager::m_ownedObjects;
LuaManager*              LuaManager::m_instance = nullptr;
LuaLibs                  LuaManager::m_libs     = LuaLibs::DEFAULT;

// Simply links into the logger to print info.
sol::object _SynginePrint(sol::variadic_args va) {
    std::string output = "[Lua] ";
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
        } else if (obj.is<sol::table>()) {
            output += "table: 0x" + std::to_string(reinterpret_cast<uintptr_t>(obj.as<sol::table>().pointer()));
        } else if (obj.is<sol::function>()) {
            output +=
                "function: 0x" + std::to_string(reinterpret_cast<uintptr_t>(
                                     obj.as<sol::function>().pointer()));
        } else if (obj.is<sol::userdata>()) {
            output +=
                "userdata: 0x" + std::to_string(reinterpret_cast<uintptr_t>(
                                     obj.as<sol::userdata>().pointer()));
        } else if (obj.is<sol::lua_nil_t>()) {
            output += "nil";
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
    gameObjectType["name"] = sol::property(
        [](Syngine::GameObject& self) -> const std::string& {
            return self.name;
        },
        [](Syngine::GameObject& self, const std::string& value) {
            self.name = value;
        });
    gameObjectType["type"] = sol::property(
        [](Syngine::GameObject& self) -> const std::string& {
            return self.type;
        },
        [](Syngine::GameObject& self, const std::string& value) {
            self.type = value;
        });
    gameObjectType["enabled"] = sol::property(
        [](Syngine::GameObject& self) -> bool {
            return self.enabled;
        },
        [](Syngine::GameObject& self, bool value) {
            self.enabled = value;
        });
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
        } else if (normalizedType == "ZoneComponent") {
            if (args.size() < 3) {
                Logger::LogF(LogLevel::ERR,
                             "Invalid arguments for adding ZoneComponent in Lua. Expected (shape, position, size[, oneShot]).");
                return sol::lua_nil;
            }

            ZoneShape shape = ZoneShape::BOX;
            std::string shapeStr = "BOX";
            float pos[3] = {0.0f, 0.0f, 0.0f};
            float size[3] = {1.0f, 1.0f, 1.0f};
            bool oneShot = false;

            // Parse args in order: (shape, position, size[, oneShot])
            if (args.size() == 4 && args[0].is<std::string>() &&
                args[1].is<sol::table>() && args[2].is<sol::table>() &&
                args[3].is<bool>()) {
                shapeStr = args[0].as<std::string>();
                // Defaults to box so only check sphere
                if (shapeStr == "BOX") {
                    shape = ZoneShape::BOX;
                } else if (shapeStr == "SPHERE") {
                    shape = ZoneShape::SPHERE;
                } else {
                    Logger::LogF(LogLevel::ERR,
                                 "Invalid zone shape '%s' specified in Lua. Defaulting to BOX.",
                                 shapeStr.c_str());
                }
                sol::table posTable = args[1].as<sol::table>();
                sol::table sizeTable = args[2].as<sol::table>();
                for (size_t i = 0; i < 3; ++i) {
                    pos[i] = posTable.get_or(i + 1, pos[i]);
                    size[i] = sizeTable.get_or(i + 1, size[i]);
                }
                oneShot = args[3].as<bool>();
            } else if (args.size() == 3 && args[0].is<std::string>() &&
                       args[1].is<sol::table>() && args[2].is<sol::table>()) {
                shapeStr = args[0].as<std::string>();
                if (shapeStr == "BOX") {
                    shape = ZoneShape::BOX;
                } else if (shapeStr == "SPHERE") {
                    shape = ZoneShape::SPHERE;
                } else {
                    Logger::LogF(LogLevel::ERR,
                                 "Invalid zone shape '%s' specified in Lua. Defaulting to BOX.",
                                 shapeStr.c_str());
                }
                sol::table posTable = args[1].as<sol::table>();
                sol::table sizeTable = args[2].as<sol::table>();
                for (size_t i = 0; i < 3; ++i) {
                    pos[i] = posTable.get_or(i + 1, pos[i]);
                    size[i] = sizeTable.get_or(i + 1, size[i]);
                }
            } else {
                Logger::LogF(LogLevel::ERR,
                             "Invalid arguments for adding ZoneComponent in Lua. Expected (shape, position, size[, oneShot]) with correct types.");
                return sol::lua_nil;
            }

            ZoneComponent* comp = obj->AddComponent<ZoneComponent>(shape, pos, size, oneShot);
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
    scene["createGameObject"] =
        [&lua](std::string                name,
               sol::optional<std::string> shader,
               sol::optional<std::string> tag) -> sol::object {
        // Create a new GameObject
        GameObject* go =
            new GameObject(name, shader.value_or("default"), tag.value_or(""));
        m_ownedObjects.push_back(go); // Track ownership for cleanup
        return sol::make_object(lua, go);
    };
    scene["deleteGameObject"] = [](GameObject* obj) {
        if (obj) {
            delete obj;
        }
    };
    scene["getGameObject"] = [](std::string name, sol::this_state ts) {
        GameObject* obj = Registry::GetGameObjectByName(name);
        if (obj) {
            sol::state_view lua(ts);
            return sol::make_object(lua, obj);
        }
        return sol::object(sol::lua_nil);
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

struct _LuaKeybind {
    InputAction* m_action;
    sol::optional<sol::function> m_onPressed = sol::nullopt;
    sol::optional<sol::function> m_onReleased = sol::nullopt;
    sol::optional<sol::function> m_onStateChanged = sol::nullopt;

    _LuaKeybind& operator=(const _LuaKeybind&) = delete;
    _LuaKeybind(const _LuaKeybind&) = delete;

    _LuaKeybind& operator=(_LuaKeybind&&) = default;
    _LuaKeybind(_LuaKeybind&&) = default;

    _LuaKeybind(const KeyBinding& binding = KeyBinding()) {
        static long long count = 0;

        m_action = InputAction::RegisterAction("_LUA_INTERNAL_BINDING_DO_NOT_USE_" + std::to_string(count),
            "", "_INTERNAL", binding, {
                .onPressed = [this]() -> void {
                    if (m_onPressed.has_value()) m_onPressed.value()();
                },
                .onReleased = [this]() -> void {
                    if (m_onReleased.has_value()) m_onReleased.value()();
                },
                .onStateChanged = [this]() -> void {
                    if (m_onStateChanged.has_value()) m_onStateChanged.value()();
                },
            });
        ++count;
    }
};

sol::object _NewKeybindFromKey(sol::this_state lua, const std::string& key, const std::string& type) {
    KeyBinding binding;

    if (type == "keycode") {
        binding = KeyBinding(StringToKeycode(key));
    } else if (type == "scancode") {
        binding = KeyBinding(StringToScancode(key));
    } else if (type == "mouse_button") {
    } else return sol::lua_nil;
    // shortcuts and sequences should have different arguments so this is garbage

    if (binding == KeyBinding(Keycode::_UNKNOWN)
        || binding == KeyBinding(Scancode::_UNKNOWN)
        || binding == KeyBinding(MouseButton::_UNKNOWN))
        return sol::lua_nil;
    // Conversion failed, return nil instead of an object in a strange state

    auto result = std::make_shared<_LuaKeybind>(binding);
    return sol::make_object(lua, result);
}

sol::object _NewKeybindFromCompound(sol::this_state lua, const sol::table& key, const std::string& type) {
    if (type == "shortcut") {

    } else if (type == "sequence") {

    } else return sol::lua_nil;
    // keycodes, scancodes, and mouse buttons should have different arguments so this is garbage

    return sol::lua_nil; // TODO: Implement shortcuts and sequences
}

void LuaManager::_RegisterInputBindings(sol::state& lua) {
    auto keybind = lua.new_usertype<_LuaKeybind>("Keybind");

    keybind["isPressed"] = sol::property(
        [](_LuaKeybind& self) -> bool {
            return self.m_action->isPressed();
        }
    );

    keybind["wasPressed"] = sol::property(
        [](_LuaKeybind& self) -> bool {
            return self.m_action->wasPressed();
        }
    );

    keybind["wasReleased"] = sol::property(
        [](_LuaKeybind& self) -> bool {
            return self.m_action->wasReleased();
        }
    );

    keybind["stateChanged"] = sol::property(
        [](_LuaKeybind& self) -> bool {
            return self.m_action->stateChanged();
        }
    );

    keybind["onPressed"] = sol::property(
        [](_LuaKeybind& self) -> sol::optional<sol::function> {
            return self.m_onPressed;
        },
        [](_LuaKeybind& self, const sol::optional<sol::function>& callback) -> void {
            self.m_onPressed = callback;
        }
    );

    keybind["onReleased"] = sol::property(
        [](_LuaKeybind& self) -> sol::optional<sol::function> {
            return self.m_onReleased;
        },
        [](_LuaKeybind& self, const sol::optional<sol::function>& callback) -> void {
            self.m_onReleased = callback;
        }
    );

    keybind["onStateChanged"] = sol::property(
        [](_LuaKeybind& self) -> sol::optional<sol::function> {
            return self.m_onStateChanged;
        },
        [](_LuaKeybind& self, const sol::optional<sol::function>& callback) -> void {
            self.m_onStateChanged = callback;
        }
    );
}

// Custom require function that looks for Lua scripts in the "scripts" directory
sol::object _CustomRequire(sol::this_state ts, const std::string& moduleName) {
    sol::state_view lua(ts);
    if (moduleName.empty()) {
        return sol::lua_nil;
    }

    // Remove any .lua extension from the module name if present
    std::string sanitizedModuleName = moduleName;
    if (sanitizedModuleName.size() >= 4 &&
        sanitizedModuleName.substr(sanitizedModuleName.size() - 4) == ".lua") {
        sanitizedModuleName = sanitizedModuleName.substr(0, sanitizedModuleName.size() - 4);
    }
    // Remove any directory traversal attempts (../ or ..\) from the module name
    while (true) {
        size_t pos = sanitizedModuleName.find("../");
        if (pos == std::string::npos) {
            break;
        }
        sanitizedModuleName.erase(pos, 3);
    }
    while (true) {
        size_t pos = sanitizedModuleName.find("..\\");
        if (pos == std::string::npos) {
            break;
        }
        sanitizedModuleName.erase(pos, 3);
    }

    // Construct the full path to the Lua script in the "scripts" directory
    std::string fullModuleName =  _GetAppDataPath("scripts/" + sanitizedModuleName + ".lua").string();
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
            base["setmetatable"] = nullptr;
            base["getmetatable"] = nullptr;
        }
        if (removeCoroutine) {
            lua["coroutine"] = nullptr;
        }

        /* sol::object _NewKeybind(sol::this_state lua, const std::string& key) {
    return _NewKeybindFromKey(lua, key, "keycode");
} */

        // Add in our own
        if (!noSyngine) {
            lua["syngine"] = lua.create_table_with("log", _SynginePrint);
            lua["syngine"]["newKeybind"] = sol::overload(
                &_NewKeybindFromKey,
                &_NewKeybindFromCompound,
                [](sol::this_state lua, const std::string& key) -> sol::object {
                    return _NewKeybindFromKey(lua, key, "keycode");
                }
            );
            _RegisterEntityBindings(lua);
            _RegisterInputBindings(lua);
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
    m_instance    = this;
    m_libs        = args;
}

LuaManager::~LuaManager() {
    if (m_luaState) {
        delete m_luaState;
        m_luaState = nullptr;
        m_instance = nullptr;
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

// Yeah I know it literally just deletes and recreates the Lua state inplace but
// it's simple and effective
void LuaManager::_ReloadLuaState() {
    Logger::Log("Reloading Lua state...");

    // Clean up owned GameObjects
    for (GameObject* obj : m_ownedObjects) {
        delete obj;
    }
    m_ownedObjects.clear();

    // Recreate the Lua state
    if (m_luaState) {
        delete m_luaState;
        m_luaState = nullptr;
    }

    m_initialized = false;
    m_allowTicking = true; // Re-enable ticking on reload
    m_luaState    = new sol::state();

    if (!m_luaState) {
        Logger::Error("Failed to recreate Lua state during reload.");
        return;
    }

    // Lambda to help figure out which libraries to close
    auto has = [](LuaLibs f) {
        return (static_cast<uint32_t>(m_libs) & static_cast<uint32_t>(f)) != 0;
    };

    // Re-open libraries based on the stored flags
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

    Logger::Log("Lua state reloaded successfully.");
}

bool LuaManager::HasObject(const std::string& name) {
    if (!m_luaState) {
        Logger::Error("Lua state is not initialized. Cannot check for object.");
        return false;
    }

    sol::object obj = (*m_luaState)[name];
    return obj.valid() && obj.get_type() != sol::type::lua_nil;
}

void LuaManager::DoFunction(const std::string& funcName, const void* data, size_t dataSize) {
    if (!m_luaState) {
        Logger::Error("Lua state is not initialized. Cannot call function.");
        return;
    }

    if (!m_allowTicking) {
        return;
    }

    sol::function func = (*m_luaState)[funcName];
    if (!func.valid()) {
        Logger::LogF(LogLevel::WARN,
                     "Lua function '%s' does not exist. Skipping call.",
                     funcName.c_str());
        return;
    }

    std::vector<char> args = std::vector<char>((char*)data, (char*)data + dataSize);
    sol::protected_function_result result = func(sol::as_args(args));
    if (!result.valid()) {
        sol::error err = result;
        Logger::LogF(LogLevel::ERR,
                     "Lua error during function call '%s': %s",
                     funcName.c_str(),
                     err.what());
        m_allowTicking = false; // Prevent further calls to Lua functions until reload to avoid spamming errors
    }
}

// This is pretty much a special overload of DoFunction without logging and fixed numerical args
void LuaManager::DoTick(float physDeltaTime, float realDeltaTime, bool isSimulating) {
    if (!m_luaState) {
        return;
    }

    if (!m_allowTicking) {
        return;
    }

    sol::function func = (*m_luaState)["onTick"];
    if (!func.valid()) {
        return;
    }

    sol::protected_function_result result = func(physDeltaTime, realDeltaTime, isSimulating);
    if (!result.valid()) {
        sol::error err = result;
        Logger::LogF(LogLevel::ERR,
                     "Lua error during function call 'onTick': %s",
                     err.what());
        m_allowTicking = false; // Prevent further calls to Lua functions until reload to avoid spamming errors
    }
}

} // namespace Syngine
