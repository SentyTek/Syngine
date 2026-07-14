# Syngine API Documentation

## ZoneManager.h header

[<- Back](../index.md)

[See source](./../../src/Syngine/Core/ZoneManager.h)

ZoneManager is responsible for managing all ZoneComponents in the game. @section ZoneManager @internal

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

- [Member Variables](#member-variables)

## Additional Functions: 

### Functions: 

- [_RegisterZone()](#zonemanager-_registerzone)
- [_UnregisterZone()](#zonemanager-_unregisterzone)
- [_UpdateZones()](#zonemanager-_updatezones)
- [GetZones()](#zonemanager-getzones)

---
<a id="zonemanager-_registerzone"></a>

#### **`ZoneManager::_RegisterZone()`**

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
<a id="zonemanager-_unregisterzone"></a>

#### **`ZoneManager::_UnregisterZone()`**

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
<a id="zonemanager-_updatezones"></a>

#### **`ZoneManager::_UpdateZones()`**

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
<a id="zonemanager-getzones"></a>

#### **`ZoneManager::GetZones()`**

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
