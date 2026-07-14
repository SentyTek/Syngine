# Syngine API Documentation

## FsUtils.h header

[<- Back](../index.md)

[See source](./../../src/Syngine/Utils/FsUtils.h)

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

### Functions: 

- [_GetAppDataPath()](#syngine-_getappdatapath)
- [_ResolveOSPath()](#syngine-_resolveospath)
- [_CheckRequiredFolders()](#syngine-_checkrequiredfolders)
- [_MakeRelativeToRoot()](#syngine-_makerelativetoroot)
- [_FileExists()](#syngine-_fileexists)

---
<a id="syngine-_getappdatapath"></a>

#### **`Syngine::_GetAppDataPath()`**

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
<a id="syngine-_resolveospath"></a>

#### **`Syngine::_ResolveOSPath()`**

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
<a id="syngine-_checkrequiredfolders"></a>

#### **`Syngine::_CheckRequiredFolders()`**

 Checks if required folders exist in the game directory

#### This function is internal use only and not intended for public use!

Signature:
```cpp
inline bool _CheckRequiredFolders(bool headless);
```
**Parameters:**
- `headless`: Whether the engine is running in headless mode (if true, some folders may not be required)

**Returns:** True if all required folders exist, false otherwise

**This function has been available since:** v0.0.1

---
<a id="syngine-_makerelativetoroot"></a>

#### **`Syngine::_MakeRelativeToRoot()`**

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
<a id="syngine-_fileexists"></a>

#### **`Syngine::_FileExists()`**

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
