# Build & Install Instructions for Syngine & Testgame (Bakerman)

## Dependencies 
### Windows
- Windows 10 22H2 or later
- Visual Studio 2022 with the Desktop C++ Kit & Windows 10/11 SDK installed
- CMake 3.16 or later
- Ninja 1.12.1 or later

### macOS
- macOS 15.2 or later
- Xcode 16 or later
- CMake 3.16 or later

### Linux
- I don't know what distros will and will not work. Probably doesn't matter if you build from source
- I just know Syngine only supports X11 currently
- Ninja 1.12 or later
- CMake 3.16 or later

## Install
### Windows
- Clone or download the repository
- Move the downloaded folder somewhere so the file structure is like this:
  ```
  /myProject
    /build
    /engine (This is the cloned repo)
  ```
- In a regular command line in the `engine/` folder, run `git submodule update --init --recursive` to download all third party addons. This may take a moment
- Move the `engine/testgame` folder to the `myProject` folder, so it is outside of `engine`
- Open the `x64 Developer Command Prompt for VS 2022` from Windows Search
- Navigate to the build folder of your project (`myProject`) and run these commands in the Dev Cmd:
  - `cmake .. -G Ninja`
  - `ninja`
- Once it's done building (it may take a few minutes), you can open and run `bakerman.exe` in the `build/Debug` folder.
  - Ninja may present an error about BX_CONFIG_DEBUG not being defined. This is fine, delete the erroring if statement.
- You can also change to Release configuration by using `cmake -DCMAKE_BUILD_TYPE=Release` and rerunning `ninja`

### macOS
- Clone or download the repository
- Move the downloaded folder somewhere so the file structure is like this:
  ```
  /myProject
    /build
    /engine (This is the cloned repo)
  ```
- In Terminal in the `engine/` folder, run `git submodule update --init --recursive` to download all third party addons. This may take a moment
- Move the `engine/testgame` folder to the `myProject` folder, so it is outside of `engine`
- In the `build` directory of your project, run `cmake .. -G Xcode` in Terminal. We use Xcode to generate the final project.
- Open the `.xcodeproj` file in the `build` directory. It should open Xcode.
- Press `cmd + b` to build, and once that's finished **RERUN** Cmake and **REBUILD** Xcode! This is crucial to ensure shaders get properly built!
  - Xcode may present an error about BX_CONFIG_DEBUG not being defined. This is fine, delete the erroring if statement.
- Press `cmd + r` in Xcode to run, or open the compiled `bakerman.app` app bundle in the `build/Debug` folder.

### Linux
- Clone or download the repository
- Move the downloaded folder somewhere so the file structure is like this:
  ```
  /myProject
    /build
    /engine (This is the cloned repo)
  ```
- In Terminal in the `engine/` folder, run `git submodule update --init --recursive` to download all third party addons. This may take a moment
- Move the `engine/testgame` folder to the `myProject` folder, so it is outside of `engine`
- Navigate to the `build` folder of your project (`myProject`) and run these commands in Terminal:
  - `cmake .. -G Ninja`
  - `ninja`
- Once it's done building (it may take a few minutes), you can open and run `bakerman` in the `build/Debug` folder.
  - Ninja may present an error about BX_CONFIG_DEBUG not being defined. This is fine, delete the erroring if statement.
- You can also change to Release configuration by using `cmake -DCMAKE_BUILD_TYPE=Release` and rerunning `ninja`