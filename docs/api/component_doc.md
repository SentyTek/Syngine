# Syngine API Documentation

## Component.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/ECS/Component.h)

Syngine::Component is the base class for all components that can be attached to GameObject to inherit from. They are used to add functionality to GameObjects, such as mesh, transform, physics, AI, PlayerController, etc. @section Component

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

### Enums and Structs: 

- [DefaultComponents](#syngine-defaultcomponents)

### Functions: 

- [GetComponentType()](#component-getcomponenttype)
- [Clone()](#component-clone)
- [Serialize()](#component-serialize)

---
<a id="syngine-defaultcomponents"></a>

#### **`Syngine::DefaultComponents()`**

 An enum of built-in component types.

Signature:
```cpp
enum DefaultComponents : ComponentTypeID
```
**Members:**
| Name | Description |
| --- | --- | 
| `SYN_COMPONENT_MESH` | 3D mesh or model |
| `SYN_COMPONENT_TRANSFORM` | Position, rotation, scale |
| `SYN_COMPONENT_AI` | AI logic/behavior |
| `SYN_COMPONENT_PLAYER` | Player controller |
| `SYN_COMPONENT_CAMERA` | Camera logic |
| `SYN_COMPONENT_LIGHT` | Lighting |
| `SYN_COMPONENT_AUDIO_EMIT` | Audio source |
| `SYN_COMPONENT_AUDIO_LISTEN` | Audio listener |
| `SYN_COMPONENT_PARTICLE` | Particle system |
| `SYN_COMPONENT_UI` | UI elements |
| `SYN_COMPONENT_SCRIPT` | Custom scripting |
| `SYN_COMPONENT_ANIMATION` | Animation controller |
| `SYN_COMPONENT_RIGIDBODY` | Physics rigidbody |
| `SYN_COMPONENT_PHYSICS_MOTOR` | Physics motor/actuator |
| `SYN_COMPONENT_PHYSICS_CLOTH` | Cloth simulation |
| `SYN_COMPONENT_PHYSICS_JOINT` | Physics joint/constraint |
| `SYN_COMPONENT_TERRAIN` | Terrain |
| `SYN_COMPONENT_ZONE` | Trigger zone or area |
| `SYN_COMPONENT_BILLBOARD` | Billboard component |
| `SYN_COMPONENT_COUNT` | Total number of component types |
---
<a id="component-getcomponenttype"></a>

#### **`Component::GetComponentType()`**

 Get the type of the component, used for identifying the component type

Signature:
```cpp
 virtual ComponentTypeID GetComponentType() = 0;
```
**Returns:** The type of the component, as defined in the Components enum

---
<a id="component-clone"></a>

#### **`Component::Clone()`**

 Clone the component, used for copying components when duplicating GameObjects or when copying components from one GameObject to another

Signature:
```cpp
 virtual std::unique_ptr<Component> Clone() const = 0;
```
**Returns:** A unique pointer to the cloned component

---
<a id="component-serialize"></a>

#### **`Component::Serialize()`**

 Serialize the component to a data node

Signature:
```cpp
 virtual Serializer::DataNode Serialize() const = 0;
```
**Returns:** A pointer to the serialized data node representing the component's state

---
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `bool` | `isEnabled` | Whether the component is enabled or not |
| `GameObject*` | `m_owner` | The owner of the component, the GameObject it is attached to |
---
