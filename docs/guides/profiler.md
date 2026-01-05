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

The profiler is currently always available in Syngine. No special build flags are required.

---

## 2. Instrumenting Code
First, be sure to do `#include "Syngine/Syngine.h"` or `#include "Syngine/Utils/Profiler.h"` in any file you wish to use the Profiler in.

### Profile an Entire Function

The easiest and recommended way to use the Profiler is to use the `SYN_PROFILE_FUNCTION()` macro at the top of a function to automatically use the function name:

```cpp
void UpdatePhysics() {
	SYN_PROFILE_FUNCTION();
	// ... physics update code ...
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

