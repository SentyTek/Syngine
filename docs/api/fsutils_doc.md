# Syngine API Documentation


## FsUtils.h header


[<- Back](../index.md)

[See source](./../../src/Syngine/Utils/FsUtils.h)

---

## Goto: 


- [_GetAppDataPath](#syngine_getappdatapath)
- [_ResolveOSPath](#syngine_resolveospath)
- [_CheckRequiredFolders](#syngine_checkrequiredfolders)
- [_MakeRelativeToRoot](#syngine_makerelativetoroot)
- [_FileExists](#syngine_fileexists)

---

#### **`Syngine::_GetAppDataPath`**


 Get the path to the application data directory for the specified application.

#### This function is internal use only and not intended for public use!


**Note:** This function does take into account the platform-specific differences in the application data directory structure.

Signature:

```cpp
std::filesystem::path _GetAppDataPath(const std::string& appName);
```

**Parameters:**

- `appName`: The name of the application

**Returns:** The path to the application data directory

**This function has been available since:** v0.0.1

---

#### **`Syngine::_ResolveOSPath`**


 Resolves the full relative path to resourced files based on platform. macOS uses a different path structure than others because of the app bundle

#### This function is internal use only and not intended for public use!


Signature:

```cpp
static inline std::string _ResolveOSPath(const char* path);
```

**Parameters:**

- `path`: The relative path to the resource file

**Returns:** The full path to the resource file, adjusted for the platform

**This function has been available since:** v0.0.1

---

#### **`Syngine::_CheckRequiredFolders`**


 Checks if required folders exist in the game directory

#### This function is internal use only and not intended for public use!


Signature:

```cpp
inline bool _CheckRequiredFolders();
```

**Returns:** True if all required folders exist, false otherwise

**This function has been available since:** v0.0.1

---

#### **`Syngine::_MakeRelativeToRoot`**


 Take an absolute path and make it relative to the root of the game directory

**Note:** Assumes the input path is to something within the game directory. If it's not, the behavior is undefined.

Signature:

```cpp
static inline std::string _MakeRelativeToRoot(std::string path);
```

**Parameters:**

- `path`: The absolute path to convert

**Returns:** The path relative to the root of the game directory

**This function has been available since:** v0.0.1

---

#### **`Syngine::_FileExists`**


 Check if a file exists at the given path, taking into account platform-specific path resolution

Signature:

```cpp
bool _FileExists(const char* path);
```

**Parameters:**

- `path`: The relative path to the file to check

**Returns:** True if the file exists, false otherwise

**This function has been available since:** v0.0.1

---

