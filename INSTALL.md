# Build & Install Instructions for Syngine & Testgame (Bakerman)

## Dependencies 
### Windows
- Windows 10 22H2 or later
- Visual Studio 2022 with the Desktop C++ Kit & Windows 10/11 SDK installed
- CMake 3.16 or later
- Ninja 1.12.1 or later

### macOS
The project requires the Xcode toolchain, even if you're editing with a different IDE
- macOS 15.2 or later
- Xcode 16 or later
- CMake 3.16 or later
- Ninja 1.12.1 or later

### Linux
- I don't know what distros will and will not work. Probably doesn't matter if you build from source
- I just know Syngine only supports X11 currently
- Ninja 1.12 or later
- CMake 3.16 or later

## Install
### Windows
- Clone or download the Bakerman repository
- Move the downloaded folder somewhere so the file structure is like this:
  ```
  Bakerman (The cloned Bakerman repo)
       ├─ build
       ├─ game
       ├─ editor
       ╰─ engine (This repository)
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
  Bakerman (The cloned Bakerman repo)
       ├─ build
       ├─ game
       ├─ editor
       ╰─ engine (This repository)
  ```
- In Terminal in the `engine/` folder, run `git submodule update --init --recursive` to download all third party addons. This may take a moment

#### Editing with Xcode
- Create two new directories in the `build` directory, `Ninja` and `Xcode`
- In the `Xcode` directory, run `cmake -S ../.. -G Xcode` in Terminal.
- Navigate to the `build/Ninja` directory in Terminal and run `cmake -S ../.. -G Ninja`.
- Open the `.xcodeproj` file in the `build/Xcode` directory. It should open Xcode.
- Navigate to the project settings, the root object in the file tree.
- Create a new build target, select `External Build Tool`, name it `bakerman-build`, set the command to the install location of Ninja, and the run location to `build/Ninja`.
- In Xcode, select everything except the project root in the file tree, right click and select `delete`, and select `Remove references`.
- Select the `engine`, `editor`, `game`, `assets`, and `CMakeLists.txt` files and folders in the `Bakerman` directory, drag them into the Xcode project under the root, select `reference in place`, and add them to the `bakerman-build` target.
- Run `where ninja` in Terminal and note the output. This is the location of the Ninja build tool on your computer.
- Click on the build scheme in Xcode's top center status bar and click on `Edit schemes`.
- Go to the build tab and remove all the targets, replacing them with the one you just created. Save the scheme, we'll come back to it later.
- Run `ninja` in the `build/Ninja` directory in Terminal to build the project.
- Go back to the scheme editor in Xcode and in the run tab, change the executable to run, select other, and choose `Bakerman/build/Ninja/Debug/Bakerman.app`. If this does not exist, you may have to build the project with `cmd + B` in Xcode. Do not choose `build/Ninja/Bakerman.app`, that is a template used by the build system.
- You should now be able to build with `cmd + B` and run and debug under LLDB in Xcode with `cmd + R`.
  - Xcode may present an error about BX_CONFIG_DEBUG not being defined. This is fine, delete or comment out the erroring if statement.

#### Editing with an external IDE
- In Terminal in the `build` folder, run `cmake -S .. -G Ninja`, and run `ninja` to build the project.
- You can open the application from the `build/Debug` folder.

### Linux
- Clone or download the repository
- Move the downloaded folder somewhere so the file structure is like this:
  ```
  Bakerman (The cloned Bakerman repo)
       ├─ build
       ├─ game
       ├─ editor
       ╰─ engine (This repository)
  ```
- In a terminal in the `engine/` folder, run `git submodule update --init --recursive` to download all third party addons. This may take a moment
- Move the `engine/testgame` folder to the `myProject` folder, so it is outside of `engine`
- Navigate to the `build` folder of your project (`myProject`) and run these commands in Terminal:
  - `cmake .. -G Ninja`
  - `ninja`
- Once it's done building (it may take a few minutes), you can open and run `bakerman` in the `build/Debug` folder.
  - Ninja may present an error about BX_CONFIG_DEBUG not being defined. This is fine, delete the erroring if statement.
- You can also change to Release configuration by using `cmake -DCMAKE_BUILD_TYPE=Release` and rerunning `ninja`