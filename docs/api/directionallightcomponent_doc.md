# Syngine API Documentation

## DirectionalLightComponent.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/GameObjects/Components/DirectionalLightComponent.h)

Syngine Directional Light Component. The DirectionalLightComponent is used to represent a directional light source in the 3D game world, such as the sun. It provides lighting information for rendering and shading. @section DirectionalLightComponent @note There can only be two directional light components in the scene at any given time. If a third directional light component is added, it will be ignored and a warning will be logged.

**This class has been available since:** v0.0.2. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 

- [Member Variables](#member-variables)

## Additional Functions: 

### Constructors: 

- [DirectionalLightComponent(GameObject* owner, const Math::Vec3& direction, const Math::Vec3& color, float intensity)](#directionallightcomponent-directionallightcomponentgameobject-owner-const-math-vec3-direction-const-math-vec3-color-float-intensity)

### Functions: 

- [GetComponentType()](#directionallightcomponent-getcomponenttype)
- [Clone()](#directionallightcomponent-clone)
- [Serialize()](#directionallightcomponent-serialize)
- [SetDirection()](#directionallightcomponent-setdirection)
- [GetDirection()](#directionallightcomponent-getdirection)
- [GetDirectionVector()](#directionallightcomponent-getdirectionvector)
- [SetDirectionVector()](#directionallightcomponent-setdirectionvector)
- [SetColor()](#directionallightcomponent-setcolor)
- [GetColor()](#directionallightcomponent-getcolor)
- [SetIntensity()](#directionallightcomponent-setintensity)
- [GetIntensity()](#directionallightcomponent-getintensity)

---
<a id="directionallightcomponent-getcomponenttype"></a>

#### **`DirectionalLightComponent::GetComponentType()`**

 Get the type of this component

Signature:
```cpp
 virtual ComponentTypeID GetComponentType() override;
```
**Returns:** The component type as an enum value

**This function has been available since:** v0.0.2

---
<a id="directionallightcomponent-clone"></a>

#### **`DirectionalLightComponent::Clone()`**

 Clone the component, used for copying components when duplicating GameObjects

Signature:
```cpp
 virtual std::unique_ptr<IComponent> Clone() const override;
```
**Returns:** A unique pointer to a new instance of the component with the same state

---
<a id="directionallightcomponent-serialize"></a>

#### **`DirectionalLightComponent::Serialize()`**

 Serialize the component to a data node for saving or networking

Signature:
```cpp
 virtual Serializer::DataNode Serialize() const override;
```
**Returns:** A data node representing the component's state

**This function has been available since:** v0.0.2

---
<a id="directionallightcomponent-directionallightcomponentgameobject-owner-const-math-vec3-direction-const-math-vec3-color-float-intensity"></a>

## Class Constructor

#### **`DirectionalLightComponent(GameObject* owner, const Math::Vec3& direction, const Math::Vec3& color, float intensity)`**

 Construct a new Directional Light Component object

Signature:
```cpp
 DirectionalLightComponent(GameObject* owner, const Math::Vec3& direction, const Math::Vec3& color, float intensity);
```
**Parameters:**
- `owner`: Pointer to the owner GameObject
- `direction`: The initial direction of the light in world space
- `color`: The initial color of the light (RGB)
- `intensity`: The initial intensity (brightness) of the light

**This function has been available since:** v0.0.2

---
<a id="directionallightcomponent-setdirection"></a>

#### **`DirectionalLightComponent::SetDirection()`**

 Set the light direction using Euler angles.

**Note:** Roll is ignored because a direction vector cannot represent roll.

Signature:
```cpp
 inline void SetDirection(const Math::Vec3& direction);
```
**Parameters:**
- `direction`: Euler angles in radians: (pitch, yaw, roll).

**This function has been available since:** v0.0.2

---
<a id="directionallightcomponent-getdirection"></a>

#### **`DirectionalLightComponent::GetDirection()`**

 Get the light direction as Euler angles.

**Note:** Roll is always 0 because the light stores only a direction vector.

Signature:
```cpp
 inline Math::Vec3 GetDirection() const;
```
**Returns:** Euler angles in radians: (pitch, yaw, 0).

**This function has been available since:** v0.0.2

---
<a id="directionallightcomponent-getdirectionvector"></a>

#### **`DirectionalLightComponent::GetDirectionVector()`**

 Get the light's world-space direction vector.

Signature:
```cpp
 inline const Math::Vec3& GetDirectionVector() const;
```
**Returns:** A normalized direction vector representing the light's direction.

**This function has been available since:** v0.0.2

---
<a id="directionallightcomponent-setdirectionvector"></a>

#### **`DirectionalLightComponent::SetDirectionVector()`**

 Set the light's world-space direction vector directly.

Signature:
```cpp
 inline void SetDirectionVector(const Math::Vec3& direction);
```
**Parameters:**
- `direction`: A normalized or approximately normalized direction vector.

**This function has been available since:** v0.0.2

---
<a id="directionallightcomponent-setcolor"></a>

#### **`DirectionalLightComponent::SetColor()`**

 Set the color of the light (RGB)

Signature:
```cpp
 inline void SetColor(const Math::Vec3& color);
```
**Parameters:**
- `color`: The new color of the light (RGB)

**This function has been available since:** v0.0.2

---
<a id="directionallightcomponent-getcolor"></a>

#### **`DirectionalLightComponent::GetColor()`**

 Get the color of the light (RGB)

Signature:
```cpp
 inline const Math::Vec3& GetColor() const;
```
**Returns:** The color of the light (RGB)

**This function has been available since:** v0.0.2

---
<a id="directionallightcomponent-setintensity"></a>

#### **`DirectionalLightComponent::SetIntensity()`**

 Set the intensity (brightness) of the light

Signature:
```cpp
 inline void SetIntensity(float intensity);
```
**Parameters:**
- `intensity`: The new intensity (brightness) of the light

**This function has been available since:** v0.0.2

---
<a id="directionallightcomponent-getintensity"></a>

#### **`DirectionalLightComponent::GetIntensity()`**

 Get the intensity (brightness) of the light

Signature:
```cpp
 inline float GetIntensity() const;
```
**Returns:** The intensity (brightness) of the light

**This function has been available since:** v0.0.2

---
## Member Variables

| Type | Name | Description |
| --- | --- | --- | 
| `Math::Vec3` | `m_direction` | Light rotation (orientation) in world space |
| `float` | `m_intensity` | Light intensity (brightness) |
| `bool` | `m_enabled` | Whether the light is enabled or disabled |
---
