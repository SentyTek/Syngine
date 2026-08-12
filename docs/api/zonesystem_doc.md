# Syngine API Documentation

## ZoneSystem.h header

[<- Back](../index.md)

[See source](./../../src/Syngine/Scene/ZoneSystem.h)

ZoneSystem is responsible for tracking all ZoneComponents in the game, checking for GameObjects entering or exiting zones, and triggering events as necessary. @section ZoneSystem @internal

---
## Goto: 


## Additional Functions: 

### Functions: 

- [_RegisterZone()](#zonesystem-_registerzone)
- [_UnregisterZone()](#zonesystem-_unregisterzone)
- [_UpdateZones()](#zonesystem-_updatezones)
- [GetZones()](#zonesystem-getzones)

---
<a id="zonesystem-_registerzone"></a>

#### **`ZoneSystem::_RegisterZone()`**

 Registers a zone with the manager.

#### This function is internal use only and not intended for public use!

**Note:** This is called automatically when a ZoneComponent is created.

Signature:
```cpp
 static void _RegisterZone(ZoneComponent* zone) noexcept;
```
**Parameters:**
- `zone`: The zone to register.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**This function will not throw exceptions.**

---
<a id="zonesystem-_unregisterzone"></a>

#### **`ZoneSystem::_UnregisterZone()`**

 Unregisters a zone from the manager.

#### This function is internal use only and not intended for public use!

**Note:** This is called automatically when a ZoneComponent is destroyed.

Signature:
```cpp
 static void _UnregisterZone(ZoneComponent* zone) noexcept;
```
**Parameters:**
- `zone`: The zone to unregister.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**This function will not throw exceptions.**

---
<a id="zonesystem-_updatezones"></a>

#### **`ZoneSystem::_UpdateZones()`**

 Updates all zones, checking for GameObjects entering or exiting zones and triggering events as necessary.

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static void _UpdateZones();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**Throws:** IDK a runtime error maybe lol

---
<a id="zonesystem-getzones"></a>

#### **`ZoneSystem::GetZones()`**

 Gets all registered zones.

Signature:
```cpp
 static const std::vector<ZoneComponent*>& GetZones();
```
**Returns:** A const reference to the vector of registered zones.

---
