# Syngine Documentation
# Getting Started Guide
Welcome to the official getting started guide for Syngine. We'll show you how to download, set up, and compile your first game with Syngine

## Table of Contents
- [Prerequisites](#prerequisites)
- [Install](#install)
- [Icon](#icon)
- [Building](#building)
- [Conclusion](#conclusion)

## Prerequisites
The following is required to build Syngine:
- Windows 10/macOS 26 or later
- C++20 standard
- CMake
- Git
- Ninja is highly recommended
- On macOS, Xcode is required

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

And that's it! Syngine is now successfully installed, and your project is ready to work on. Last thing
Your IDE may warn you about Syngine files not being found. This is normal. After building, place the generated `compile_commands.json` file from `build/` into your project's root folder. (If a `compile_commands.json` file was not generated, re-run CMake with the `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` flag)

## Desktop Setup
### Icon
One last step: Creating an app icon:
- Windows: Use any icon maker app to open an image and generate a `.ico` file, this will be the Windows icon
- macOS: Use Icon Composer (macOS 26+) to create an icon, then Save (File -> Save) it as a `.icon` bundle
- Linux: Use a `.png` file of at least 512x512

The ico, icon, and/or PNG file(s) live in `system/assets`. Ensure the name is either `projectName.icon` for macOS (i.e. `bakerman.icon`) or `icon.ico`/`icon.png` for Windows/Linux.

### Assets.xcassets
For macOS, we also need an `Assets.xcassets` folder. Xcodebuild needs this folder to properly incorporate the app icon. Do not make this folder yourself. This step is only required on macOS.

On macOS, use the `assets_xcassets.sh` script in `engine/docs` folder. This will create the folder and all required files.

We also recommend changing your app's accent color by opening the `Assets.xcassets` bundle in Xcode

### Info.plist
This is a macOS only step.
The install scripts will automatically generate an Info.plist file, however some things still need to be updated by editing `system/assets/Info.plist.in`.

We recommend updating the copyright, bundle versions, and the development region, if you are developing in a different region  


## Building

Now it is time to build (or compile, whatever you say) the project. Building the project is fortunately just a few commands. You can either use your IDEs built in CMake controller or the command line.

We use CMake to build Syngine & Syngine games. In a terminal instance in the `build` directory of the project, run the following command to configure the compiler: `cmake .. -G Ninja` If you use another build system than Ninja, feel free to swap the generator. In addition, Debug mode may be turned on by adding `-DCMAKE_BUILD_TYPE:STRING=Debug` to the command. Some users may prefer to use the CMake extension for their IDE of choice, and that should work fine too with your regular workflow. Building Syngine with Visual Studio has not been tested.

This may take a few minutes depending on your machine.

After it's configured, simply run `ninja` in the build directory to compile. This may take a few minutes. Please do not turn off your computer.

**After it's built, you may need to rerun the CMake command and Ninja to compile shaders**, but this will only take a second.

After it is built, you can run the executable or app bundle found in either the `build/bin` folder or the `build/{release_mode}` folder alongside some tool executables. If all is good, the game should launch immediately and without presenting any errors, and the window should be titled your game, displaying a skydome.

## Conclusion
Congratulations, you've just built your first project with Syngine! Now, you can view the [API Reference](index.md#api-reference) to learn more on what features Syngine has at your disposal.