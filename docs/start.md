# Syngine Documentation
# Getting Started Guide
Welcome to the official getting started guide for Syngine. We'll show you how to download, set up, and compile your first game with Syngine

## Table of Contents
- [Prerequisites](#prerequisites)
- [Install](#install)
- [Shaders](#shader-creation)
- [Icon](#icon)
- [Building](#building)
- [Conclusion](#conclusion)

## Prerequisites
The following is required to build Syngine:
- C++20 standard
- CMake
- Git
- Ninja is highly recommended

We recommend compiling with either MSVC or Clang, depending on platform.

## Install

The easiest way to download Syngine is to use our helpful setup tool. This setup tool, found in `docs` folder (`install_unix.sh` and `install_win.bat`):
- Clone or Download the repository such that your file structure is as so:
```
    myGameProject/
    |   engine/ (This is where you put the extracted Syngine)
    |   |   include/
    |   |   src/
    |   |   default/
    |   |   docs/
    |   |   |   install_unix.sh
    |   |   |   install_win.bat
    |   |   etc
```
- After downloading and matching the above file structure, simply double click the install script for your platform.
- This script should have created several more directories in the `myGameProject/` folder (or whatever you called it)

Next, open a terminal in the engine folder of the project,and run the following command to download the remaining third party softwares: `git submodule update --init --recursive` This command may take a few minutes depending on your internet connection.

And that's it! Syngine is now successfully installed, and your project is ready to work on. Before we get the project running though, we need to add a Shader. Shaders are what tell the GPU how to draw things. Syngine comes with a few, but we still need to make one to tie everything together.

## Shader Creation
In `myGameProject/assets/shaders` (which should be an empty folder except for `varying`), create files `sky.frag.sc` and `sky.vert.sc`, and in `varying` folder create `sky.vary.sc`. `sky` is the overall name of the shader.

Shader creation is a simple three step process:
- Writing the shader
- Loading the program in C++
- Compiling the shader

Every shader Syngine uses requires a Vertex and Fragment shader like most engines, but we also require a Varying shader, used to pass data between the vertex and fragment shaders. Combined, all three are called a Program. The shader language is similar to GLSL, however with a few tweaks which are mostly listed [here](https://bkaradzic.github.io/bgfx/tools.html#shader-compiler-shaderc). Following is an example sky shader, ready to paste into the files created earlier.

sky.frag.sc
```glsl
$input v_worldViewRay
#include <bgfx_shader.sh>

uniform vec4 u_lightDir;
uniform vec4 u_skyColorDay;
uniform vec4 u_skyColorNight;
uniform vec4 u_sunColorDay;
uniform vec4 u_sunColorRise;

void main() {
    float SUN_ELEVATION_DEEP_TWILIGHT_END = -0.309;

    vec3 lightDir = normalize(u_lightDir.xyz);

    float sunElevation = lightDir.y;
    float dayIntensity = smoothstep(SUN_ELEVATION_DEEP_TWILIGHT_END, 0.2, sunElevation);
    vec3 skyColor = mix(u_skyColorNight.rgb, u_skyColorDay.rgb, dayIntensity);

    if (sunElevation > SUN_ELEVATION_DEEP_TWILIGHT_END) { //only draw sun if its near or above the horizon
        float cosAngle = dot(normalize(v_worldViewRay), lightDir);
        float sunColorIntensity = smoothstep(SUN_ELEVATION_DEEP_TWILIGHT_END + 0.2, 0.2, sunElevation);
        vec3 sunColor = mix(u_sunColorRise.rgb, u_sunColorDay.rgb, sunColorIntensity);

        //sun disk - adjust 0.999 to change size. closer to 1 equals smaller sun
        //cos(0.5d) ~ 0.99996
        //cos(1d) ~ 0.9998
        //cos(2d) ~ 0.9993
        float sunDiskThreshold = 0.999; //2.5d
        float sunEdgeSoftness = 0.0001;

        //sun core
        float sunCoreIntensity = smoothstep(sunDiskThreshold, sunDiskThreshold + sunEdgeSoftness, cosAngle);

        //sun glow
        float sunGlowRadius = 0.005;
        float sunGlowIntensity = smoothstep(sunDiskThreshold - sunGlowRadius, sunDiskThreshold, cosAngle);

        //compile and mix
        vec3 glowColorContrib = sunColor.rgb * 0.5 + skyColor * 0.5;
        skyColor = mix(skyColor, glowColorContrib, sunGlowIntensity);
        skyColor = mix(skyColor, sunColor.rgb, sunCoreIntensity);
    }

    gl_FragColor = vec4(skyColor.rgb, 1.0);
}
```

sky.vert.sc
```glsl
$output v_worldViewRay
#include <bgfx_shader.sh>

void main() {
    float x, y;
    if(gl_VertexID == 0) {
        x = -1.0;
        y = -1.0;
    } else if(gl_VertexID == 1) {
        x = 3.0;
        y = -1.0;
    } else {
        x = -1.0;
        y = 3.0;
    }
    gl_Position = vec4(x, y, 1.0, 1.0);

    vec4 ndcPos = gl_Position;
    vec4 viewPos = mul(u_invProj, ndcPos);
    viewPos /= viewPos.w;

    vec3 worldDir = mul(u_invView, vec4(viewPos.xyz, 0.0)).xyz;
    v_worldViewRay = worldDir;
}
```

sky.vary.sc
```glsl
vec3 a_position : POSITION;

vec3 v_worldViewRay : TEXCOORD1;
```

### Linking the Shaders
In `game/src/main.cpp`, this is the code that Syngine will actually execute. This is your game. The installer preloads it with the bare minimum for it to work, but we need to link the shader we just created. The engine only knows about the shaders it created, not ones we've made, so we need to tell the engine about out shaders.

**Creating programs and uniforms should be done before the while loop, but after `engine.Initialize()` is called.**

The Render API (`Syngine/Renderer.h`) has a helpful function to load in shaders, `Renderer::AddProgram`. We can give it the path to the shader, `shaders/sky`, and tell it when to render (render view):

```cpp
size_t skyProgram =
    Renderer::AddProgram("shaders/sky", "sky", Syngine::VIEW_SKY);
if (skyProgram == -1) {
    Syngine::Logger::Fatal("Failed to create sky program");
    return -1;
}
```

Then, we need to add uniforms. Uniforms are basically variables in the shader, passed to it from our C++ code. The engine needs to know about these uniforms so it can use them properly. The sky shader has four uniforms, to set the sky and sun colors during day and night. First, we need to tell the engine about the uniforms, matching the declared name and type in the shader, then we can assign them to something. For example:

```cpp
size_t UniformID1 = Renderer::RegisterUniform(program, "u_skyColorDay" UniformType::UNIFORM_VEC4);
```

Notice how this matches the name and type in the shader:
```glsl
uniform vec4 u_skyColorDay;
```

Then, we can set the uniform.
```cpp
float skyColorDay[4] = { 0.5f, 0.7f, 1.0f, 1.0f };
Renderer::SetUniform(UniformID1, skyColorDay);
```
And put it all together, repeating for all four uniforms.
```cpp
    size_t id1 = Renderer::RegisterUniform(skyProgram, "u_skyColorDay", UniformType::UNIFORM_VEC4);
    size_t id2 = Renderer::RegisterUniform(skyProgram, "u_skyColorNight", UniformType::UNIFORM_VEC4);
    size_t id3 = Renderer::RegisterUniform(skyProgram, "u_sunColorDay", UniformType::UNIFORM_VEC4);
    size_t id4 = Renderer::RegisterUniform(skyProgram, "u_sunColorRise", UniformType::UNIFORM_VEC4);

    float skyColorDay[4] = { 0.5f, 0.7f, 1.0f, 1.0f };
    float skyColorNight[4] = { 0.0f, 0.0f, 0.1f, 1.0f };
    float sunColorDay[4]   = { 1.0f, 1.0f, 0.9f, 1.0f };
    float sunColorRise[4]  = { 0.77f, 0.39f, 0.14f, 1.0f };

    Renderer::SetUniform(id1, skyColorDay);
    Renderer::SetUniform(id2, skyColorNight);
    Renderer::SetUniform(id3, sunColorDay);
    Renderer::SetUniform(id4, sunColorRise);
```

Again, this code should be run after engine initialization but before the while loop. If you want, you can call `Renderer::SetUniform` with new parameters during the while loop to say change the sky color during runtime, or perhaps with player input.

Now only one more thing: telling the compiler about the shaders. Fortunately, the installer already did this. Any shaders created in the `assets/shaders` folder are automatically recognized by the compiler.

## Icon
One last step: Creating an app icon:
- Windows: Use any icon maker app to open an image and generate a `.ico` file, this will be the Windows icon
- macOS: Use Icon Composer (macOS 26+) to create an icon, then export it as a `.icon` bundle
- Linux: Use a `.png` file of at least 512x512

The ico, icon, and/or PNG file(s) live in `system/assets`. Ensure the name is either `projectName.icon` for macOS (i.e. `bakerman.icon`) or `icon.ico`/`icon.png` for Windows/Linux.

### Assets.xcassets
For macOS, we also need an `Assets.xcassets` folder. Xcodebuild needs this folder to properly incorporate the app icon. Do not make this folder yourself. This step is only required on macOS.

On macOS, use the `assets_xcassets.sh` script in `engine/docs` folder. This will create the folder and all required files.

## Building

Now it is time to build (or compile, whatever you say) the project. Building the project is fortunately just a few commands.

We use CMake to build Syngine & Syngine games. In a terminal instance in the `build` directory of the project, run the following command to configure the compiler: `cmake .. -G Ninja` If you use another build system than Ninja, feel free to swap the generator. In addition, Debug mode may be turned on by adding `-DCMAKE_BUILD_TYPE:STRING=Debug` to the command. Some users may prefer to use the CMake extension for their IDE of choice, and that should work fine too with your regular workflow. Building Syngine with Visual Studio has not been tested.

This may take a few minutes depending on your machine. If it warns about missing shader directories, that's fine.

After it's configured, simply run `ninja` in the build directory to compile. This may take a few minutes. Please do not turn off your computer.

Chances are **after it's built, you'll need to rerun the CMake command and Ninja to compile shaders**, but this will only take a second.

After it is built, you can run the executable or app bundle found in either the `build/bin` folder. If all is good, the game should launch immediately and without presenting any errors, and the window should be pure blue, as that is the sky shader default color.

## Conclusion
Congratulations, you've just built your first project with Syngine! Now, you can view the [API Reference](index.md#api-reference) to learn more on what features Syngine has at your disposal.