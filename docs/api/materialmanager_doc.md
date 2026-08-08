# Syngine API Documentation

## MaterialManager.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Graphics/Resources/MaterialManager.h)

Immutable material asset. A material owns the shader and its default values; per-renderer changes belong on a MaterialInstance. Mutable values used by a single renderer. Unset values resolve to the immutable defaults held by Material.

---
## Goto: 


## Additional Functions: 

### Constructors: 

- [Material(const std::string& name)](#material-materialconst-std-string-name)

<a id="material-materialconst-std-string-name"></a>

## Class Constructor

#### **`Material(const std::string& name)`**



**Note:** Highly recommended to NOT use this constructor directly.

Signature:
```cpp
 Material(const std::string& name) : name(name), shader(nullptr);
```
---
