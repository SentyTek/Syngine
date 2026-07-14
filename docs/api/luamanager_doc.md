# Syngine API Documentation

## LuaManager.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Core/LuaManager.h)

Manages the Lua state and scripting environment for the engine. @section Lua

<div style="background:#08082e; padding:15px; border-radius:8px; margin-bottom:20px; font-family:sans-serif;">
    <label for="mdSearch" style="font-weight:bold; display:block; margin-bottom:5px;">Search Functions:</label>
    <input type="text" id="mdSearch" placeholder="Type function name..." onkeyup="filterMarkdownDocs()" style="width:100%; padding:8px; border:1px solid #ccc; border-radius:4px; font-size:16px; background-color: rgb(60, 60, 60); color:antiquewhite">
</div>

<script>
function filterMarkdownDocs() {
    var input = document.getElementById('mdSearch').value.toLowerCase();
    // Targets common markdown containers or the whole document body
    var elements = document.querySelectorAll('h1, h2, h3, h4, p, li, pre, hr');

    elements.forEach(function(el) {
        // Skip the search box itself
        if (el.closest('#mdSearch') || el.id === 'mdSearch') return;

        var text = el.innerText.toLowerCase();
        if (text.includes(input)) {
            el.style.display = ""; // Show matching element
            //el.style.backgroundColor = input ? "#fff9c4" : ""; // Highlight if searching
        } else {
            el.style.display = input ? "none" : ""; // Hide if it doesn't match
        }
    });
}
</script>

---
## Goto: 


## Additional Functions: 

### Constructors: 

- [LuaManager(LuaLibs args = LuaLibs::DEFAULT)](#luamanager-luamanagerlualibs-args-lualibs-default)

### Enums and Structs: 

- [LuaLibs](#syngine-lualibs)

### Functions: 

- [_UnregisterLuaOwnedObject()](#luamanager-_unregisterluaownedobject)
- [SafeScript()](#luamanager-safescript)
- [SafeFile()](#luamanager-safefile)
- [AddFunction()](#luamanager-addfunction)
- [HasObject()](#luamanager-hasobject)
- [DoFunction()](#luamanager-dofunction)
- [DoTick()](#luamanager-dotick)
- [SetAllowTicking()](#luamanager-setallowticking)
- [GetLastResult()](#luamanager-getlastresult)

---
<a id="syngine-lualibs"></a>

#### **`Syngine::LuaLibs()`**

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
<a id="luamanager-_unregisterluaownedobject"></a>

#### **`LuaManager::_UnregisterLuaOwnedObject()`**

 Unregister a GameObject from Lua ownership tracking (called automatically on GameObject destruction)

Signature:
```cpp
 static void _UnregisterLuaOwnedObject(GameObject* obj);
```
**Parameters:**
- `obj`: The GameObject to unregister

---
<a id="luamanager-luamanagerlualibs-args-lualibs-default"></a>

## Class Constructor

#### **`LuaManager(LuaLibs args = LuaLibs::DEFAULT)`**

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
<a id="luamanager-safescript"></a>

#### **`LuaManager::SafeScript()`**

 Do a safe script execution with error handling

Signature:
```cpp
 static void SafeScript(const std::string& script);
```
**Parameters:**
- `script`: The Lua script to execute

**This function has been available since:** v0.0.1

---
<a id="luamanager-safefile"></a>

#### **`LuaManager::SafeFile()`**

 Do a safe file execution with error handling

Signature:
```cpp
 static void SafeFile(const std::string& filePath);
```
**Parameters:**
- `filePath`: The path to the Lua file to execute

**This function has been available since:** v0.0.1

---
<a id="luamanager-addfunction"></a>

#### **`LuaManager::AddFunction()`**

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

**This function has been available since:** v0.0.1

---
<a id="luamanager-hasobject"></a>

#### **`LuaManager::HasObject()`**

 Check if the Lua state has an object with the given name

Signature:
```cpp
 static bool HasObject(const std::string& name);
```
**Parameters:**
- `name`: The name of the object to check for

**Returns:** True if the object exists in the Lua state, false otherwise

**This function has been available since:** v0.0.1

---
<a id="luamanager-dofunction"></a>

#### **`LuaManager::DoFunction()`**

 Does a function if it exists in the Lua state, with the given arguments

Signature:
```cpp
 static void DoFunction(const std::string& funcName, const void* data, size_t dataSize);
```
**Parameters:**
- `funcName`: The name of the function to call
- `data`: Pointer to the data to pass as arguments
- `dataSize`: Size of the data in bytes

**This function has been available since:** v0.0.1

---
<a id="luamanager-dotick"></a>

#### **`LuaManager::DoTick()`**

 Ticks the Lua state by calling the "onTick" function if it exists, with the given delta time

Signature:
```cpp
 static void DoTick(float physDeltaTime, float realDeltaTime, bool simulating);
```
**Parameters:**
- `physDeltaTime`: The time elapsed since the last physics tick, in seconds
- `realDeltaTime`: The real time elapsed since the last tick, in seconds (not affected by time scaling)
- `simulating`: Whether the simulation is currently running

**This function has been available since:** v0.0.1

---
<a id="luamanager-setallowticking"></a>

#### **`LuaManager::SetAllowTicking()`**

 Set whether to allow ticking Lua scripts (used to prevent ticking during reloads)

Signature:
```cpp
 static inline bool SetAllowTicking(bool allow);
```
**Parameters:**
- `allow`: True to allow ticking, false to prevent it

**Returns:** The previous allow ticking state

**This function has been available since:** v0.0.1

---
<a id="luamanager-getlastresult"></a>

#### **`LuaManager::GetLastResult()`**

 Get the last result from a Lua function call, converted to type T

**Note:** This will return of a FUNCTION CALL ONLY, make sure DoFunction has been called at least once before this on a function with a return value. Otherwise, this returns nothing.

Signature:
```cpp
 template <typename T> static T GetLastResult();
```
**Template Parameters:**
- `T`: The type to convert the result to

**Returns:** The converted result, or default T if conversion failed

**This function has been available since:** v0.0.2

---
