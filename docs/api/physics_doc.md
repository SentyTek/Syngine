# Syngine API Documentation


## Physics.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Physics/Physics.h)

---

## Goto: 


- [Phys](#physphys)
- [_Init](#phys_init)
- [_Shutdown](#phys_shutdown)
- [_Update](#phys_update)
- [_GetDebugRenderer](#phys_getdebugrenderer)
- [_DrawDebug](#phys_drawdebug)
- [_GetBodyInterface](#phys_getbodyinterface)
- [_GetPhysicsSystem](#phys_getphysicssystem)
- [_GetBroadPhaseLayerInterface](#phys_getbroadphaselayerinterface)
- [_GetObjectLayerPairFilter](#phys_getobjectlayerpairfilter)
- [_GetObjectVsBroadPhaseLayerFilter](#phys_getobjectvsbroadphaselayerfilter)
- [_CreateSphere](#phys_createsphere)
- [_CreateBox](#phys_createbox)
- [_CreateMeshBody](#phys_createmeshbody)
- [_CreateCapsule](#phys_createcapsule)
- [_CreateCylinder](#phys_createcylinder)

---

#### **`Phys::Phys`**


 Physics manager class for Syngine

Signature:

```cpp
 public: Phys();
```

**This function has been available since:** v0.0.1

---

#### **`Phys::_Init`**


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

#### **`Phys::_Shutdown`**


 Shutdown the physics system

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 void _Shutdown();
```

**This function has been available since:** v0.0.1

---

#### **`Phys::_Update`**


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

#### **`Phys::_GetDebugRenderer`**


 Get the debug renderer

Signature:

```cpp
 DebugRender* _GetDebugRenderer();
```

**Returns:** Pointer to the debug renderer

---

#### **`Phys::_DrawDebug`**


 Draw debug information

Signature:

```cpp
 void _DrawDebug(int width, int height, bgfx::ProgramHandle program, Syngine::Camera camera, Syngine::Camera finalCam, DebugModes debug);
```

**Parameters:**

- `width`: Width of the viewport
- `height`: Height of the viewport
- `program`: Shader program to use for rendering

---

#### **`Phys::_GetBodyInterface`**


 Get the body interface for manipulating bodies

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 BodyInterface& _GetBodyInterface();
```

**Returns:** Reference to the body interface

**This function has been available since:** v0.0.1

---

#### **`Phys::_GetPhysicsSystem`**


 Get the physics system

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 PhysicsSystem& _GetPhysicsSystem();
```

**Returns:** Reference to the physics system

**This function has been available since:** v0.0.1

---

#### **`Phys::_GetBroadPhaseLayerInterface`**


 Get the broad phase layer interface

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 const BPLayerInterfaceImpl& _GetBroadPhaseLayerInterface() const;
```

**Returns:** Reference to the broad phase layer interface

**This function has been available since:** v0.0.1

---

#### **`Phys::_GetObjectLayerPairFilter`**


 Get the object vs broad phase layer filter

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 const ObjectLayerPairFilterImpl& _GetObjectLayerPairFilter() const;
```

**Returns:** Reference to the object vs broad phase layer filter

**This function has been available since:** v0.0.1

---

#### **`Phys::_GetObjectVsBroadPhaseLayerFilter`**


 Get the object vs broad phase layer filter

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 const ObjectVsBroadPhaseLayerFilterImpl& _GetObjectVsBroadPhaseLayerFilter() const;
```

**Returns:** Reference to the object vs broad phase layer filter

**This function has been available since:** v0.0.1

---

#### **`Phys::_CreateSphere`**


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

#### **`Phys::_CreateBox`**


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

#### **`Phys::_CreateMeshBody`**


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

#### **`Phys::_CreateCapsule`**


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

#### **`Phys::_CreateCylinder`**


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

