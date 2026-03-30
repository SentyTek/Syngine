# Syngine API Documentation


## ZoneManager.h header


[<- Back](../index.md)

[See source](./../../src/Syngine/Core/ZoneManager.h)

---

## Goto: 


- [Member Variables](#member-variables)
- [_RegisterZone](#zonemanager_registerzone)
- [_UnregisterZone](#zonemanager_unregisterzone)
- [_UpdateZones](#zonemanager_updatezones)
- [GetZones](#zonemanagergetzones)

---

#### **`ZoneManager::_RegisterZone`**


 ZoneManager is responsible for managing all ZoneComponents in the game.

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 public: /// @brief Registers a zone with the manager. /// @param zone The zone to register. /// @since v0.0.1 /// @threadsafety not-safe /// @internal /// @note This is called automatically when a ZoneComponent is created. /// @noexcept static void _RegisterZone(ZoneComponent* zone) noexcept;
```

---

#### **`ZoneManager::_UnregisterZone`**


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

#### **`ZoneManager::_UpdateZones`**


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

#### **`ZoneManager::GetZones`**


 Gets all registered zones.

Signature:

```cpp
 static const std::vector<ZoneComponent*>& GetZones();
```

**Returns:** A const reference to the vector of registered zones.

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- | 
| `std::vector<ZoneComponent*>` | `m_zones` | List of all zones in the game |

---

