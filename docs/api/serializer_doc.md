# Syngine API Documentation


## Serializer.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Utils/Serializer.h)

---

## Goto: 


- [c](#serializerc)
- [using](#datanodeusing)
- [Type](#datanodetype)
- [Has](#datanodehas)
- [Operator [] overload](#datanodeoperator[]overload)
- [Operator = overload](#datanodeoperator=overload)
- [Operator = overload](#datanodeoperator=overload)
- [std::string](#datanodestd::string)
- [Operator / overload](#datanodeoperator/overload)
- [As](#datanodeas)
- [Set](#datanodeset)
- [GetType](#datanodegettype)
- [IsObject](#datanodeisobject)
- [IsArray](#datanodeisarray)
- [IsNull](#datanodeisnull)
- [Size](#datanodesize)
- [At](#datanodeat)
- [Append](#datanodeappend)
- [GetKeys](#datanodegetkeys)
- [CoreSettings](#datanodecoresettings)
- [Prefab](#datanodeprefab)
- [Scene](#datanodescene)
- [SaveData](#datanodesavedata)
- [_ReadFromBundle](#datanode_readfrombundle)
- [_ParseFloatArray](#datanode_parsefloatarray)
- [_ParseIntArray](#datanode_parseintarray)
- [_ParseStringArray](#datanode_parsestringarray)
- [_LoadCoreSettings](#datanode_loadcoresettings)
- [_SaveCoreSettings](#datanode_savecoresettings)
- [_LoadCoreSettingsCategory](#datanode_loadcoresettingscategory)

---

#### **`Serializer::c`**


 Utility class for serializing and deserializing engine and game data to disk.

Signature:

```cpp
 public: // Helper types for serialization struct Float3 { float x, y, z; };
```

**This function has been available since:** v0.0.1

---

#### **`DataNode::using`**


 A flexible data node that can represent various data types and hierarchical structures (like JSON or XML).

Signature:

```cpp
 public: using NodeMap = std::unordered_map<std::string, DataNode>;
```

**This function has been available since:** v0.0.1

---

#### **`DataNode::Type`**


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

#### **`DataNode::Has`**


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

#### **`Operator [] overload`**


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

#### **`Operator = overload`**


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

#### **`Operator = overload`**


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

#### **`std::string`**


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

#### **`Operator / overload`**


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

#### **`DataNode::As`**


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

#### **`DataNode::Set`**


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

#### **`DataNode::GetType`**


 Gets the type of data stored in the DataNode

Signature:

```cpp
 Type GetType() const;
```

**Returns:** The Type enum value representing the data type

**This function has been available since:** v0.0.1

**See also:**

-  Serializer::DataNode::Type

---

#### **`DataNode::IsObject`**


 Checks if the DataNode is of a specific type

Signature:

```cpp
 bool IsObject() const;
```

**Returns:** True if the DataNode is an Object type

**This function has been available since:** v0.0.1

---

#### **`DataNode::IsArray`**


 Checks if the DataNode is of a specific type

Signature:

```cpp
 bool IsArray() const;
```

**Returns:** True if the DataNode is an Array type

**This function has been available since:** v0.0.1

---

#### **`DataNode::IsNull`**


 Checks if the DataNode is null/empty

Signature:

```cpp
 bool IsNull() const;
```

**Returns:** True if the DataNode is null/empty

**This function has been available since:** v0.0.1

---

#### **`DataNode::Size`**


 Gets the size of the Array type DataNode

**Preconditions:** IsArray() must be true (DataNode is of Array type)

Signature:

```cpp
 size_t Size() const;
```

**Returns:** The number of elements in the array

**This function has been available since:** v0.0.1

---

#### **`DataNode::At`**


 Accesses an element of the Array type DataNode by index

**Preconditions:** IsArray() must be true (DataNode is of Array type)

Signature:

```cpp
 DataNode& At(size_t index);
```

**Parameters:**

- `index`: The index of the element to access

**Returns:** Reference to the child DataNode at the specified index

**This function has been available since:** v0.0.1

---

#### **`DataNode::Append`**


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

#### **`DataNode::GetKeys`**


 Gets all keys in the Object type DataNode. For iteration.

**Preconditions:** IsObject() must be true (DataNode is of Object type)

Signature:

```cpp
 std::vector<std::string> GetKeys() const;
```

**Returns:** Vector of keys as strings

**This function has been available since:** v0.0.1

---

#### **`DataNode::CoreSettings`**


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

#### **`DataNode::Prefab`**


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

#### **`DataNode::Scene`**


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

#### **`DataNode::SaveData`**


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

#### **`DataNode::_ReadFromBundle`**


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

#### **`DataNode::_ParseFloatArray`**


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

#### **`DataNode::_ParseIntArray`**


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

#### **`DataNode::_ParseStringArray`**


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

#### **`DataNode::_LoadCoreSettings`**


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

#### **`DataNode::_SaveCoreSettings`**


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

#### **`DataNode::_LoadCoreSettingsCategory`**


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

