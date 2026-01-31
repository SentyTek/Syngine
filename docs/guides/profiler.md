# Syngine Documentation
# Guide: Using the Syngine Profiler
[<-- Back](../index.md)

The Syngine Profiler is a lightweight, built-in tool for measuring performance hotspots in your game. It allows you to instrument code regions and functions, collect timing data per frame, and export results for analysis (e.g., in Chrome's tracing tools).

## Features

- Minimal overhead, thread-local event storage
- Easy to instrument code with macros
- Exports to Chrome Trace Event format (JSON)
- Visualize performance in Chrome or compatible tools

---

## 1. Enabling the Profiler

The profiler is enabled by default, however it may need to be turned off in certain cases:
- **The game crashes on launch without any log or trace:** This can happen when attempting to use a graphics debugger (e.g RenderDoc) with the Profiler.

To disable the profiler, head to the **engine's** CMakeLists.txt file (often in `engine/` directory), and locate the commented out code: `#target_compile_definitions(Syngine PUBLIC SYN_DEBUG_GRAPHICS)`. Uncommenting this code and rebuilding may fix these issues while disabling the profiler

**NOTE:** Any `Syngine::Profiler::*` method calls (e.g. `Profiler::Reset()`) must also be commented out when disabling the profiler to prevent compile time errors.

---

## 2. Instrumenting Code
First, be sure to do `#include "Syngine/Syngine.h"` or `#include "Syngine/Utils/Profiler.h"` in any file you wish to use the Profiler in.

### Profile an Entire Function

The easiest and recommended way to use the Profiler is to use the `SYN_PROFILE_FUNCTION()` macro at the top of a function to automatically use the function name:

```cpp
void SpawnBasketball() {
	SYN_PROFILE_FUNCTION();
	// ... code ...
}
```

### Profile a Scope

Use the `SYN_PROFILE_SCOPE(name)` macro to profile any code block or scope:

```cpp
void MyFunction() {
    /* blah */
	{
        SYN_PROFILE_SCOPE("MyFunction.DoThings");
        /* do things */
    }
    /* more blah */
}
```

### Profile Nested Scopes

You can nest scopes to see call hierarchy and timing depth:

```cpp
void RenderFrame() {
	SYN_PROFILE_FUNCTION();
	{
		SYN_PROFILE_SCOPE("ShadowPass");
		// ... shadow rendering ...
	}
	{
		SYN_PROFILE_SCOPE("LightingPass");
		// ... lighting ...
	}
}
```

---

## 3. Capturing and Saving Profile Data
To save the captured data to a file (in Chrome Trace format):

```cpp
Syngine::Profiler::ProfilerUI::SaveCapture("profile.json");
```
The file will be saved in your app's data directory (e.g., `%APPDATA%/${AppName}/profile.json`).

Saving every frame is a bad idea. It is best to only save when needed, such as using a InputAction or by frame count.

---

## 4. Visualizing Results

1. Open Chrome and navigate to `chrome://tracing`. (Or use a website such as [Perfetto](https://ui.perfetto.dev/))
2. Click "Load" and select your exported `profile.json` file.
3. Explore the timeline to analyze performance, call depths, and thread activity.

---

For more details, see the source in `engine/include/Syngine/Utils/Profiler.h` and `engine/src/Syngine/Utils/Profiler.cpp`.
