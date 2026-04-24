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
    - `.newKeybind(key: string[, type: string])` -> `Keybind` usertype — Creates a keybind from a single key. `type` can be `"keycode"` (default), `"scancode"`, or `"mouse_button"`.
    - `.newKeybind(key: string, mods: table[, type: string])` -> `Keybind` usertype — Creates a keyboard shortcut. `mods` is a table of modifier name strings. `type` can be `"keycode"` (default) or `"scancode"`.

- `game`:
    - `.setDefaultObjectsDisabled(disabled: bool)` - Enables or disables the game's default object spawning logic.

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

- `Keybind` usertype:
    - return type for `syngine.newKeybind()`
    - `.isPressed` -> bool — `true` while the key/button is held down
    - `.wasPressed` -> bool — `true` for one frame when the key/button is first pressed
    - `.wasReleased` -> bool — `true` for one frame when the key/button is released
    - `.stateChanged` -> bool — `true` for one frame whenever the pressed state changes
    - `.onPressed` — assign a callback `function()` to be called when the key/button is pressed
    - `.onReleased` — assign a callback `function()` to be called when the key/button is released
    - `.onStateChanged` — assign a callback `function()` to be called when the pressed state changes

    **Key name strings (keycode/scancode):** letters `"a"`–`"z"`, digits `"0"`–`"9"`, `"enter"`, `"escape"`, `"backspace"`, `"tab"`, `"space"`, `"delete"`, `"up"`, `"down"`, `"left"`, `"right"`, `"home"`, `"end"`, `"page_up"`, `"page_down"`, `"insert"`, `"caps_lock"`, `"num_lock"`, `"scroll_lock"`, `"print_screen"`, `"pause"`, function keys `"f1"`–`"f24"`, keypad keys `"keypad_0"`–`"keypad_9"` / `"keypad_enter"` / `"keypad_plus"` / `"keypad_minus"` / `"keypad_multiply"` / `"keypad_divide"` / `"keypad_period"` / `"keypad_equals"`, modifier keys `"left_shift"` / `"right_shift"` / `"left_control"` / `"right_control"` / `"left_alt"` / `"right_alt"` / `"left_gui"` / `"right_gui"` (aliases: `"left_command"` / `"left_windows"` etc.), and various symbols: `"space"`, `"minus"`, `"equals"`, `"left_bracket"`, `"right_bracket"`, `"semicolon"`, `"apostrophe"`, `"comma"`, `"period"`, `"slash"`, `"backslash"`, `"tilde"`, and others.

    **Mouse button name strings:** `"left"` / `"primary"`, `"right"` / `"secondary"`, `"middle"` / `"tertiary"`, `"button_4"`, `"button_5"`.

    **Modifier name strings (for shortcuts):** `"shift"`, `"left_shift"`, `"right_shift"`, `"control"`, `"left_control"`, `"right_control"`, `"alt"` / `"option"`, `"left_alt"` / `"left_option"`, `"right_alt"` / `"right_option"`, `"gui"` / `"command"` / `"windows"`, `"left_gui"` / `"left_command"` / `"left_windows"`, `"right_gui"` / `"right_command"` / `"right_windows"`.
    **NOTE FOR SHORTCUTS: "control" REQUIRES BOTH CONTROL KEYS HELD DOWN TO TRIGGER**

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
```lua
local zone = scene.createGameObject("TriggerZone")
local zoneComp = zone:AddComponent("Zone", "BOX", {x=0, y=0, z=0}, {x=5, y=3, z=5})

zoneComp:SetEntryCallback(function()
    syngine.log("Player entered the zone!")
end)

zoneComp:SetExitCallback(function()
    syngine.log("Player left the zone.")
end)
```

### Basic keybind (polling)
```lua
local jumpKey = syngine.newKeybind("space")

function onTick(physDelta, realDelta, isSimulating)
    if jumpKey.wasPressed then
        syngine.log("Jump!")
    end
end
```
Checks each frame whether Space was pressed. Use `.wasPressed` instead of `.isPressed` to fire only once per press.

### Keybind with callbacks
```lua
local fireKey = syngine.newKeybind("left", "mouse_button")

fireKey.onPressed = function()
    syngine.log("Fire!")
end

fireKey.onReleased = function()
    syngine.log("Stopped firing.")
end
```
Callbacks are invoked automatically; no polling required.

### Keyboard shortcut (key + modifiers)
```lua
-- Ctrl+R to reload something
local reloadBind = syngine.newKeybind("r", {"control"})

reloadBind.onPressed = function()
    syngine.log("Reloading...")
end
```
The second argument is a table of modifier name strings. You can combine multiple modifiers, e.g. `{"shift", "control"}`.

### Checking if a key is held
```lua
local sprintKey = syngine.newKeybind("left_shift")

function onTick(physDelta, realDelta, isSimulating)
    local speed = 5.0
    if sprintKey.isPressed then
        speed = 10.0
    end
    -- use speed ...
end
```

### Mouse button keybind
```lua
local rightClick = syngine.newKeybind("right", "mouse_button")

rightClick.onPressed = function()
    syngine.log("Right clicked!")
end
```

### Scancode-based keybind
```lua
-- Scancodes are layout-independent (physical key position, not character)
local moveForward = syngine.newKeybind("w", "scancode")

function onTick(physDelta, realDelta, isSimulating)
    if moveForward.isPressed then
        -- move forward regardless of keyboard layout
    end
end
```
