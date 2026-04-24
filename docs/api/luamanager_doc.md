# Syngine API Documentation


## LuaManager.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Core/LuaManager.h)

---

## Goto: 


- [Constructor](#class-constructor)
- [LuaLibs](#synginelualibs)
- [SafeScript](#luamanagersafescript)
- [SafeFile](#luamanagersafefile)
- [AddFunction](#luamanageraddfunction)
- [HasObject](#luamanagerhasobject)
- [DoFunction](#luamanagerdofunction)
- [DoTick](#luamanagerdotick)
- [SetAllowTicking](#luamanagersetallowticking)

---

## Class Constructor


#### **`LuaManager::LuaManager`**


 Constructor that initializes the Lua state and opens libraries based on the specified flags

**Note:** By default, DEFAULT libraries are opened if no arguments are provided. Note that supplying OS, IO, ERRHAND, it will *also* open the DEFAULT libraries, as they are required for those to function properly.

Signature:

```cpp
 LuaManager(LuaLibs args = LuaLibs::DEFAULT);
```

**Parameters:**

- `args`: Variadic arguments to specify which libraries to open (see LuaLibs enum)

**This function has been available since:** v0.0.1

---

## Class & Related Members


#### **`Syngine::LuaLibs`**


 Enum for specifying which Lua libraries to open

Signature:

```cpp
enum class LuaLibs : uint32_t
```

**Members:**

| Name | Description |
| --- | --- | 
| `NONE` | Do not open any libraries |
| `2` | Open the default set of libraries (string, utf8, math, table, require) |
| `ALL` | Open all standard Lua libraries |
| `DEBUG` | Open additional libraries useful for debugging (e.g., debug and print) |
| `IO` | Open the io library for file operations (use with caution) |
| `OS` | Open the os library for operating system interactions |
| `ERRHAND` | Open the default library for error handling and stack traces (pcall, xpcall) |
| `128` | Do not set metatables on Lua objects (for extra security) |
| `COROUTINES` | Open the coroutines library for Lua coroutines |
| `RAWFAMILY` | Open the raw* functions (rawget, rawset, etc.) for low-level table manipulation (use with caution) |
| `1024` | Do not register any Syngine bindings. This disables the ECS, logger, and input bindings, and is intended for sandboxed environments where you want to allow Lua scripting but not give access to engine internals. |

**This function has been available since:** v0.0.1

---

#### **`LuaManager::SafeScript`**


 Do a safe script execution with error handling

Signature:

```cpp
 static void SafeScript(const std::string& script);
```

**Parameters:**

- `script`: The Lua script to execute

---

#### **`LuaManager::SafeFile`**


 Do a safe file execution with error handling

Signature:

```cpp
 static void SafeFile(const std::string& filePath);
```

**Parameters:**

- `filePath`: The path to the Lua file to execute

---

#### **`LuaManager::AddFunction`**


 Add a function to the Lua state with the given name and category

Signature:

```cpp
 template <typename Func> static void AddFunction(const std::string& name, Func func, const std::string& namespace_ = "");
```

**Template Parameters:**

- `Func`: The function type (e.g., std::function<void(int)>)

**Parameters:**

- `name`: The name of the function to add
- `namespace_`: The namespace to add the function under (e.g., "game", "input", etc.)

---

#### **`LuaManager::HasObject`**


 Check if the Lua state has an object with the given name

Signature:

```cpp
 static bool HasObject(const std::string& name);
```

**Parameters:**

- `name`: The name of the object to check for

**Returns:** True if the object exists in the Lua state, false otherwise

---

#### **`LuaManager::DoFunction`**


 Does a function if it exists in the Lua state, with the given arguments

Signature:

```cpp
 static void DoFunction(const std::string& funcName, const void* data, size_t dataSize);
```

**Parameters:**

- `funcName`: The name of the function to call
- `data`: Pointer to the data to pass as arguments
- `dataSize`: Size of the data in bytes

---

#### **`LuaManager::DoTick`**


 Ticks the Lua state by calling the "onTick" function if it exists, with the given delta time

Signature:

```cpp
 static void DoTick(float physDeltaTime, float realDeltaTime, bool simulating);
```

**Parameters:**

- `physDeltaTime`: The time elapsed since the last physics tick, in seconds
- `realDeltaTime`: The real time elapsed since the last tick, in seconds (not affected by time scaling)
- `simulating`: Whether the simulation is currently running

---

#### **`LuaManager::SetAllowTicking`**


 Set whether to allow ticking Lua scripts (used to prevent ticking during reloads)

Signature:

```cpp
 static inline bool SetAllowTicking(bool allow);
```

**Parameters:**

- `allow`: True to allow ticking, false to prevent it

**Returns:** The previous allow ticking state

---

