# Syngine Documentation
# Guide: Lua Scripting in Syngine
[<-- Back](../index.md)

## Lua scripting in Syngine is a very large work in progress. This guide is currently valid for Syngine v0.0.1 as released for the One Year Technology Demo. All features are subject to change.

---

## Available Base Libraries
- `math.*`
- `string.*`
- `table.*`
- `utf8.*`
- `setmetatable`/`getmetatable`

## Added Functions
- `require(moduleName)` - Searches in `%AppData%/SentyTek/SyngineDemo/scripts` (or your OS equivalent) for a `.lua` file with the specified name.
- `syngine`:
    - `.log(message)` - Logs a message to the log file.

- `scene`:
    - `.createGameObject(name: string[, shader: string = "default", tag: string = ""])` -> `GameObject` usertype
    - `.deleteGameObject(GameObject)`
    - `.getGameObject(name: string)`

- `GameObject` usertype:
    - `.name` -> string property
    - `.type` -> string property
    - `.enabled` -> bool property
    - `:AddComponent(type: string, [constructor args])` -> `Component` usertype depends on type arg
        - type can either be `"Transform"`, `"Mesh"`, `"Rigidbody"`, `"Billboard"`, or `"Zone"` (aliases also exist, not case sensitive)
        - see [API Docs](../index.md) or below for component constructor args
    - `:GetComponent(type: string)` ->` Component` usertype depends on type arg

- `Transform` usertype:
    - return type for `AddComponent("Transform")`
    - no constructor args
    - `:SetPosition(x: float, y: float, z: float)`
    - `:SetRotationEuler(x: float, y: float, z: float)` (Euler XYZ RHS)
    - `:SetRotationQuat(x: float, y: float, z: float, w: float)`
    - `:SetScale(x: float, y: float: z: float)`
    - `:GetPosition()` -> x, y, z
    - `:GetScale()` -> x, y, z
    - `:GetRotationEuler()` -> x, y, z
    - `:GetRotationQuat()` -> x, y, z, w

- `Mesh` usertype:
    - return type for `AddComponent("Transform")`
    - constructor args: `bundlePath, meshPath[, hasTextures]`
    - no methods or properties
    
- `Rigidbody` usertype:
    - return type for `AddComponent"Rigidbody")`
    - `:AddForce(forceX: float, forceY: float, forceZ: float[, mode: string])` where `mode` can be either `"FORCE` or `IMPULSE`. Defaults to `"FORCE"`
    - `:AddForceAtPosition(forceX: float, forceY: float, forceZ: float, x: float, y: float, z: float[, mode: string])` where `mode` can be either `"FORCE"` or `"IMPULSE"`. Defaults to `"FORCE"`
    - `:AddTorque(torqueX: float, torqueY: float, torqueZ: float[, mode: string])` where `mode` can either be `"FORCE"` or `"IMPULSE"`. Defaults to `"FORCE"`

- `Billboard` usertype:
    - return type for `AddComponent("Billboard")`
    - constructor args: `bundlePath, imagePath, mode: string, size: float`
        - where mode is either `"CAMERA_ALIGNED`, `"Y_ALIGNED"`, or `"FIXED"`
    - `:SetRot(x: float, y: float, z: float)`
    - `:GetRot()` -> x, y, z

- `Zone` usertype:
    - return type for `AddComponent("Zone")`
    - constructor args: `shape: string, position: table, size: table[, oneShot: bool]`
        - where `shape` is either `"BOX"` or `"SPHERE"`, `position` is a table of `{ x, y, z }`, and `size` is a table of `{ x, y, z }`
    - `:GetShape()` -> string (either `"BOX"` or `"SPHERE"`)
    - `:GetPosition()` -> x:  float, y: float, z: float
    - `:SetPosition(x: float, y: float, z: float)`
    - `:GetSize()` -> x: float, y: float, z: float
    - `:SetSize(x: float, y: float, z: float)`
    - `:GetRotation()` -> x: float, y: float, z: float (euler XYZ)
    - `:SetRotation(x: float, y: float, z: float)` (euler XYZ)
    - `:IsActive()` -> bool
    - `:SetActive(bool)`
    - `:IsOneShot()` -> bool
    - `:AddTag(string)`
    - `:RemoveTag(string)`
    - `:HasTag(string)` -> bool
    - `:GetTags()` -> table: string
    - `:SetTags(table: string)`
    - `:IsPointInZone(x: float, y: float, z: float)` -> bool
    - `:IsObjectInZone(GameObject)` -> bool
    - `:GetObjectInZone()` -> table: GameObject
    - `:GetObjectsInZoneByTag(string)` -> table: GameObject
    - `SetEntryCallback(function)`
    - `SetExitCallback(function)`

## Examples
### Creating a renderable GameObject
```lua
local go = scene.createGameObject("TestObject")
go:AddComponent("Transform")
go:AddComponent("Mesh", "meshes/meshes.spk", "cube.glb");
```
Creates a cube object.

### Moving an object
```lua
local go = scene.createGameObject("TestObject2")
local trans = go:AddComponent("Transform")
trans:SetPosition(0.0, 10.0, 0.0)
```
Moves the object +10 on the Y axis (up). This is also valid:
```lua
local go = scene.createGameObject("TestObject2"):AddComponent("Transform"):SetPosition(0.0, 10.0, 0.0)
```

### Creating a Rigidbody
```lua
local go = scene.createGameObject("3");
local rigidbodyParams = {
    mass = 0.0, -- Highly recommended to stay at 0 to let Jolt figure it out. Don't have to, of course.
    friction = 0.7, -- 0-1
    restitution = 0.1, -- 0-1
    shape = "box",
    shapeParameters = {0.5, 0.5, 0.5}, -- 1 meter cube
    -- optional
    -- motionType = 0,
    -- layer = 1
}
go:AddComponent("Rigidbody", rigidbodyParams)
```

### Using a Zone to detect player movement
