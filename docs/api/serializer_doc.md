# Syngine API Documentation

## Serializer.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Utils/Serializer.h)

A flexible data node that can represent various data types and hierarchical structures (like JSON or XML).

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 


## Additional Functions: 

### Operator Overloads: 

- [Indexing operator overload](#indexing-operator-overload)
- [Assignment operator overload](#assignment-operator-overload)
- [Assignment operator overload](#assignment-operator-overload-2)
- [Assignment operator overload](#assignment-operator-overload-3)
- [Division operator overload](#division-operator-overload)

### Enums and Structs: 

- [Type](#datanode-type)
- [CoreSettings](#datanode-coresettings)
- [Prefab](#datanode-prefab)
- [Scene](#datanode-scene)
- [SaveData](#datanode-savedata)

### Functions: 

- [Has()](#datanode-has)
- [As()](#datanode-as)
- [Set()](#datanode-set)
- [GetType()](#datanode-gettype)
- [IsObject()](#datanode-isobject)
- [IsArray()](#datanode-isarray)
- [IsNull()](#datanode-isnull)
- [Size()](#datanode-size)
- [At()](#datanode-at)
- [Append()](#datanode-append)
- [GetKeys()](#datanode-getkeys)
- [_ReadFromBundle()](#datanode-_readfrombundle)
- [_ParseFloatArray()](#datanode-_parsefloatarray)
- [_ParseIntArray()](#datanode-_parseintarray)
- [_ParseStringArray()](#datanode-_parsestringarray)
- [_LoadCoreSettings()](#datanode-_loadcoresettings)
- [_SaveCoreSettings()](#datanode-_savecoresettings)
- [_LoadCoreSettingsCategory()](#datanode-_loadcoresettingscategory)

---
<a id="datanode-type"></a>

#### **`DataNode::Type()`**

 The type of data stored in the DataNode

Signature:
```cpp
 enum class Type
```
**Members:**
| Name | Description |
| --- | --- | 
| `Null` | No data |
| `Integer` | Integer number |
| `UnsignedInteger` | Unsigned integer number |
| `Float` | Floating-point number |
| `Boolean` | Boolean value |
| `String` | String value |
| `Object` | Key-value map |
| `Array` | Ordered list (float4, mat3, component, etc.) |
**This function has been available since:** v0.0.1

---
<a id="datanode-has"></a>

#### **`DataNode::Has()`**

 Checks if the DataNode contains a key (for Object type nodes)

Signature:
```cpp
 bool Has(const std::string& key) const;
```
**Parameters:**
- `key`: The key to check for

**Returns:** True if the key exists, false otherwise

**This function has been available since:** v0.0.1

---
<a id="indexing-operator-overload"></a>

#### **`Indexing operator overload`**

 Accesses a child DataNode by key (for Object type nodes)

**Example:** playerNode["health"] = 100;

Signature:
```cpp
 DataNode& operator[](const std::string& key);
```
**Parameters:**
- `key`: The key of the child node

**Returns:** Reference to the child DataNode

**This function has been available since:** v0.0.1

---
<a id="assignment-operator-overload"></a>

#### **`Assignment operator overload`**

 Assignment operator to copy data from another DataNode

Signature:
```cpp
 DataNode& operator=(const DataNode& other);
```
**Parameters:**
- `other`: The other DataNode to copy from

**Returns:** Reference to this DataNode

**This function has been available since:** v0.0.1

---
<a id="assignment-operator-overload-2"></a>

#### **`Assignment operator overload`**

 Assignment operator to set a value directly (int, uint64_t, float, bool, string, etc.)

Signature:
```cpp
 template <typename T> DataNode& operator=(const T& value);
```
**Template Parameters:**
- `T`: The type of the value to set (int, uint64_t, float, bool, std::string, etc.)

**Parameters:**
- `value`: The value to set

**Returns:** Reference to this DataNode

**This function has been available since:** v0.0.1

---
<a id="assignment-operator-overload-3"></a>

#### **`Assignment operator overload`**

 Assignment operator for C-style string literals to set a string value

Signature:
```cpp
 DataNode& operator=(const char* value);
```
**Parameters:**
- `value`: The C-style string literal to set

**Returns:** Reference to this DataNode

**This function has been available since:** v0.0.1

---
<a id="division-operator-overload"></a>

#### **`Division operator overload`**

 Accesses a child DataNode by index (for Array type nodes)

**Example:** playerNode / "name" / "last" = "Marston";

Signature:
```cpp
 DataNode& operator/(const std::string& value);
```
**Parameters:**
- `index`: The index of the child node

**Returns:** Reference to the child DataNode

**This function has been available since:** v0.0.1

---
<a id="datanode-as"></a>

#### **`DataNode::As()`**

 Converts the DataNode to a specific type

Signature:
```cpp
 template <typename T> T As(const T& defaultValue = T()) const;
```
**Template Parameters:**
- `T`: The type to convert to (int, float, bool, std::string, etc.)

**Parameters:**
- `defaultValue`: The default value to return if conversion fails

**Returns:** The converted value or the default value

**This function has been available since:** v0.0.1

---
<a id="datanode-set"></a>

#### **`DataNode::Set()`**

 Sets the value of the DataNode

Signature:
```cpp
 template <typename T> void Set(const T& value);
```
**Template Parameters:**
- `T`: The type of the value to set (int, float, bool, std::string, etc.)

**Parameters:**
- `value`: The value to set

**This function has been available since:** v0.0.1

---
<a id="datanode-gettype"></a>

#### **`DataNode::GetType()`**

 Gets the type of data stored in the DataNode

Signature:
```cpp
 Type GetType() const;
```
**Returns:** The Type enum value representing the data type

**This function has been available since:** v0.0.1

**See also:** [Serializer::DataNode::Type](#datanode-type)

---
<a id="datanode-isobject"></a>

#### **`DataNode::IsObject()`**

 Checks if the DataNode is of a specific type

Signature:
```cpp
 bool IsObject() const;
```
**Returns:** True if the DataNode is an Object type

**This function has been available since:** v0.0.1

---
<a id="datanode-isarray"></a>

#### **`DataNode::IsArray()`**

 Checks if the DataNode is of a specific type

Signature:
```cpp
 bool IsArray() const;
```
**Returns:** True if the DataNode is an Array type

**This function has been available since:** v0.0.1

---
<a id="datanode-isnull"></a>

#### **`DataNode::IsNull()`**

 Checks if the DataNode is null/empty

Signature:
```cpp
 bool IsNull() const;
```
**Returns:** True if the DataNode is null/empty

**This function has been available since:** v0.0.1

---
<a id="datanode-size"></a>

#### **`DataNode::Size()`**

 Gets the size of the Array type DataNode

**Preconditions:** IsArray() must be true (DataNode is of Array type)

Signature:
```cpp
 size_t Size() const;
```
**Returns:** The number of elements in the array

**This function has been available since:** v0.0.1

---
<a id="datanode-at"></a>

#### **`DataNode::At()`**

 Accesses an element of the Array type DataNode by index

**Preconditions:** IsArray() must be true (DataNode is of Array type)

Signature:
```cpp
 const DataNode& At(size_t index) const;
```
**Parameters:**
- `index`: The index of the element to access

**Returns:** Reference to the child DataNode at the specified index

**This function has been available since:** v0.0.1

---
<a id="datanode-append"></a>

#### **`DataNode::Append()`**

 Appends a new empty DataNode to the Array type DataNode

**Preconditions:** IsArray() must be true (DataNode is of Array type)

Signature:
```cpp
 void Append(DataNode& node);
```
**Parameters:**
- `node`: The DataNode to append to the array

**This function has been available since:** v0.0.1

---
<a id="datanode-getkeys"></a>

#### **`DataNode::GetKeys()`**

 Gets all keys in the Object type DataNode. For iteration.

**Preconditions:** IsObject() must be true (DataNode is of Object type)

Signature:
```cpp
 std::vector<std::string> GetKeys() const;
```
**Returns:** Vector of keys as strings

**This function has been available since:** v0.0.1

---
<a id="datanode-coresettings"></a>

#### **`DataNode::CoreSettings()`**

 Control settings like mouse sensitivity and key bindings

Signature:
```cpp
 struct CoreSettings
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `int` | `width` | Default 1080p |
| `int` | `height` | Default 1080p |
| `bool` | `fullscreen` | Windowed by default |
| `float` | `brightness` | Normal brightness |
| `bool` | `vSync` | V-Sync enabled by default |
| `bool` | `useShadows` | Shadows enabled by default |
| `int` | `shadowDist` | Shadow draw distance |
| `bool` | `useSSAO` | SSAO enabled by default |
| `DataNode` | `customSettings` | Custom video settings as DataNode tree |
| `float` | `masterVolume` | Master volume (0.0 to 1.0) |
| `float[8]` | `channels` | Volume for 8 audio channels |
| `float` | `mouseSensitivity` | Normal sensitivity |
| `bool` | `invertY` | Y-axis not inverted by default |
| `DataNode` | `binds` | Key bindings as a map of action -> key |
| `DataNode` | `Serialize()` | const Helper to serialize control settings |
| `DataNode` | `customSettings` | For game-specific settings as DataNode tree |
| `DataNode` | `Serialize()` | const Helper to serialize core settings |
**This function has been available since:** v0.0.1

---
<a id="datanode-prefab"></a>

#### **`DataNode::Prefab()`**

 Represents a prefab/asset - a reusable GameObject template with all its components

Signature:
```cpp
 struct Prefab
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `name` | Human-readable name of the prefab |
| `std::string` | `guid` | Unique identifier for the prefab |
| `DataNode` | `rootGameObjectData` | Serialized GameObject tree (with all |
| `nullptr` | `Pointer` | to the deserialized root GameObject (not |
| `false` | `Indicates` | if the prefab was successfully loaded/created |
| `void` | `WriteGameObject(const` | DataNode& node, scl::xml::XmlDocument& doc, scl::xml::XmlElem* parent) const Helper to serialize prefab |
**This function has been available since:** v0.0.1

---
<a id="datanode-scene"></a>

#### **`DataNode::Scene()`**

 Represents a complete game scene with all GameObjects and scene settings

Signature:
```cpp
 struct Scene
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `std::string` | `name` | Human-readable name of the scene |
| `std::string` | `guid` | Unique identifier for the scene |
| `DataNode` | `gameObjects` | Array of root GameObjects in the scene |
| `DataNode` | `sceneSettings` | Scene-specific settings (lighting, fog, skybox, etc.) |
| `DataNode` | `Serialize()` | const Helper to serialize scene |
**This function has been available since:** v0.0.1

---
<a id="datanode-savedata"></a>

#### **`DataNode::SaveData()`**

 Represents a complete game save file

Signature:
```cpp
 struct SaveData
```
**Members:**
| Type | Name | Description |
| --- | --- | --- | 
| `DataNode` | `state` | Serialized game state (player data, world state, etc. Game sets this up) |
| `DataNode` | `Serialize()` | const Helper to serialize save data |
**This function has been available since:** v0.0.1

---
<a id="datanode-_readfrombundle"></a>

#### **`DataNode::_ReadFromBundle()`**

 Internal helper function to read an asset from a bundle file and return it as a stream

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static inline scl::stream _ReadFromBundle(const std::string& bundlePath, const std::string& assetPath);
```
**Parameters:**
- `bundlePath`: The path to the bundle file
- `assetPath`: The path to the asset within the bundle

**Returns:** A stream containing the asset data, or an empty stream if there was an error

**This function has been available since:** v0.0.1

---
<a id="datanode-_parsefloatarray"></a>

#### **`DataNode::_ParseFloatArray()`**

 Internal helper to parse float arrays

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static inline std::vector<float> _ParseFloatArray(scl::string& value);
```
**Parameters:**
- `value`: The string value to parse (expected format: "x,y,z")

**Returns:** A vector of floats with the parsed values, or an empty vector if parsing fails

**This function has been available since:** v0.0.1

---
<a id="datanode-_parseintarray"></a>

#### **`DataNode::_ParseIntArray()`**

 Internal helper to parse int arrays

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static inline std::vector<int> _ParseIntArray(scl::string& value);
```
**Parameters:**
- `value`: The string value to parse (expected format: "x,y,z")

**Returns:** A vector of ints with the parsed values, or an empty vector if parsing fails

**This function has been available since:** v0.0.1

---
<a id="datanode-_parsestringarray"></a>

#### **`DataNode::_ParseStringArray()`**

 Internal helper to parse string arrays

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static inline std::vector<std::string> _ParseStringArray(scl::string value);
```
**Parameters:**
- `value`: The string value to parse (expected format: "str1,str2,str3")

**Returns:** A vector of strings with the parsed values, or an empty vector if parsing fails

**This function has been available since:** v0.0.1

---
<a id="datanode-_loadcoresettings"></a>

#### **`DataNode::_LoadCoreSettings()`**

 Load CoreSetting into memory

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void _LoadCoreSettings(const std::string& gameName);
```
**Parameters:**
- `gameName`: The name of the game (used to determine settings file path)

**This function has been available since:** v0.0.1

---
<a id="datanode-_savecoresettings"></a>

#### **`DataNode::_SaveCoreSettings()`**

 Save the current CoreSettings in memory to disk

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void _SaveCoreSettings(const std::string& gameName);
```
**Parameters:**
- `gameName`: The name of the game (used to determine settings file)

**This function has been available since:** v0.0.1

---
<a id="datanode-_loadcoresettingscategory"></a>

#### **`DataNode::_LoadCoreSettingsCategory()`**

 Load a section of CoreSettings

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 template <typename T> static T* _LoadCoreSettingsCategory();
```
**Template Parameters:**
- `T`: The type of the section to load (Video, Audio, Controls)

**Returns:** The loaded section data

**This function has been available since:** v0.0.1

---
