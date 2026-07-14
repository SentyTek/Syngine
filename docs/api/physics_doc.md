# Syngine API Documentation

## Physics.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Physics/Physics.h)

Physics manager class for Syngine @section Physics

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 


## Additional Functions: 

### Functions: 

- [_Init()](#phys-_init)
- [_Shutdown()](#phys-_shutdown)
- [_Update()](#phys-_update)
- [_GetDebugRenderer()](#phys-_getdebugrenderer)
- [_DrawDebug()](#phys-_drawdebug)
- [_GetBodyInterface()](#phys-_getbodyinterface)
- [_GetPhysicsSystem()](#phys-_getphysicssystem)
- [_GetBroadPhaseLayerInterface()](#phys-_getbroadphaselayerinterface)
- [_GetObjectLayerPairFilter()](#phys-_getobjectlayerpairfilter)
- [_GetObjectVsBroadPhaseLayerFilter()](#phys-_getobjectvsbroadphaselayerfilter)
- [_CreateSphere()](#phys-_createsphere)
- [_CreateBox()](#phys-_createbox)
- [_CreateMeshBody()](#phys-_createmeshbody)
- [_CreateCapsule()](#phys-_createcapsule)
- [_CreateCylinder()](#phys-_createcylinder)

---
<a id="phys-_init"></a>

#### **`Phys::_Init()`**

 Initialize the physics system

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 void _Init();
```
**Parameters:**
- `debug`: Whether to enable debug rendering

**Returns:** 0 on success, non-zero on failure

**This function has been available since:** v0.0.1

---
<a id="phys-_shutdown"></a>

#### **`Phys::_Shutdown()`**

 Shutdown the physics system

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 void _Shutdown();
```
**This function has been available since:** v0.0.1

---
<a id="phys-_update"></a>

#### **`Phys::_Update()`**

 Update the physics system

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 void _Update(float deltaTime, int collisionSteps);
```
**Parameters:**
- `deltaTime`: The time elapsed since the last update
- `collisionSteps`: The number of collision detection steps to perform

**This function has been available since:** v0.0.1

---
<a id="phys-_getdebugrenderer"></a>

#### **`Phys::_GetDebugRenderer()`**

 Get the debug renderer

Signature:
```cpp
 DebugRender* _GetDebugRenderer();
```
**Returns:** Pointer to the debug renderer

---
<a id="phys-_drawdebug"></a>

#### **`Phys::_DrawDebug()`**

 Draw debug information

Signature:
```cpp
 void _DrawDebug(int width, int height, bgfx::ProgramHandle program, Syngine::Camera camera, Syngine::Camera finalCam, DebugModes debug);
```
**Parameters:**
- `width`: Width of the viewport
- `height`: Height of the viewport
- `program`: Shader program to use for rendering
- `camera`: Camera to get debug information from
- `finalCam`: Camera to draw debug information to
- `debug`: Debug modes to determine what to draw

---
<a id="phys-_getbodyinterface"></a>

#### **`Phys::_GetBodyInterface()`**

 Get the body interface for manipulating bodies

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 BodyInterface& _GetBodyInterface();
```
**Returns:** Reference to the body interface

**This function has been available since:** v0.0.1

---
<a id="phys-_getphysicssystem"></a>

#### **`Phys::_GetPhysicsSystem()`**

 Get the physics system

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 PhysicsSystem& _GetPhysicsSystem();
```
**Returns:** Reference to the physics system

**This function has been available since:** v0.0.1

---
<a id="phys-_getbroadphaselayerinterface"></a>

#### **`Phys::_GetBroadPhaseLayerInterface()`**

 Get the broad phase layer interface

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 const BPLayerInterfaceImpl& _GetBroadPhaseLayerInterface() const;
```
**Returns:** Reference to the broad phase layer interface

**This function has been available since:** v0.0.1

---
<a id="phys-_getobjectlayerpairfilter"></a>

#### **`Phys::_GetObjectLayerPairFilter()`**

 Get the object vs broad phase layer filter

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 const ObjectLayerPairFilterImpl& _GetObjectLayerPairFilter() const;
```
**Returns:** Reference to the object vs broad phase layer filter

**This function has been available since:** v0.0.1

---
<a id="phys-_getobjectvsbroadphaselayerfilter"></a>

#### **`Phys::_GetObjectVsBroadPhaseLayerFilter()`**

 Get the object vs broad phase layer filter

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 const ObjectVsBroadPhaseLayerFilterImpl& _GetObjectVsBroadPhaseLayerFilter() const;
```
**Returns:** Reference to the object vs broad phase layer filter

**This function has been available since:** v0.0.1

---
<a id="phys-_createsphere"></a>

#### **`Phys::_CreateSphere()`**

 Create a sphere body

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 BodyID _CreateSphere(RVec3Arg position, float radius, EMotionType motionType, ObjectLayer layer, float mass = 0.0f);
```
**Parameters:**
- `position`: Position of the sphere
- `radius`: Radius of the sphere
- `motionType`: Motion type of the sphere
- `layer`: Layer of the sphere
- `mass`: Mass of the

**Returns:** Body ID of the created sphere

**This function has been available since:** v0.0.1

---
<a id="phys-_createbox"></a>

#### **`Phys::_CreateBox()`**

 Create a box body

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 BodyID _CreateBox(RVec3Arg position, QuatArg rotation, Vec3Arg halfExtent, EMotionType motionType, ObjectLayer layer, float mass = 0.0f);
```
**Parameters:**
- `position`: Position of the box
- `rotation`: Rotation of the box
- `halfExtent`: Half extent of the box
- `motionType`: Motion type of the box
- `layer`: Layer of the box
- `mass`: Mass of the box

**Returns:** Body ID of the created box

**This function has been available since:** v0.0.1

---
<a id="phys-_createmeshbody"></a>

#### **`Phys::_CreateMeshBody()`**

 Create a mesh body

#### This function is internal use only and not intended for public use!

**Note:** The mesh data should be in the Syngine::MeshData format

Signature:
```cpp
 BodyID _CreateMeshBody(RVec3Arg position, QuatArg rotation, const MeshData& meshData, EMotionType motionType, ObjectLayer layer, const JPH::Vec3& scale = JPH::Vec3(1.0f, 1.0f, 1.0f), const float mass = 0.f);
```
**Parameters:**
- `position`: Position of the mesh
- `rotation`: Rotation of the mesh
- `meshData`: Mesh data to create the body from
- `motionType`: Motion type of the mesh
- `layer`: Layer of the mesh
- `scale`: Scale of the mesh

**Returns:** Body ID of the created mesh

**This function has been available since:** v0.0.1

---
<a id="phys-_createcapsule"></a>

#### **`Phys::_CreateCapsule()`**

 Create a capsule body

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 BodyID _CreateCapsule(RVec3Arg position, float radius, float halfHeight, EMotionType motionType, ObjectLayer layer, float mass = 0.0f);
```
**Parameters:**
- `position`: Position of the capsule
- `radius`: Radius of the capsule
- `halfHeight`: Half height of the capsule
- `motionType`: Motion type of the capsule
- `layer`: Layer of the capsule
- `mass`: Mass of the capsule

**Returns:** Body ID of the created capsule

**This function has been available since:** v0.0.1

---
<a id="phys-_createcylinder"></a>

#### **`Phys::_CreateCylinder()`**

 Create a cylinder body

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 BodyID _CreateCylinder(RVec3Arg position, QuatArg rotation, float radius, float halfHeight, EMotionType motionType, ObjectLayer layer, float mass = 0.0f);
```
**Parameters:**
- `position`: Position of the cylinder
- `rotation`: Rotation of the cylinder
- `radius`: Radius of the cylinder
- `halfHeight`: Half height of the cylinder
- `motionType`: Motion type of the cylinder
- `layer`: Layer of the cylinder
- `mass`: Mass of the cylinder

**Returns:** Body ID of the created cylinder

**This function has been available since:** v0.0.1

---
