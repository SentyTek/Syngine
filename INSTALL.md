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
- In Terminal in the `/engine` folder, run `git submodule update --init --recursive` to download all third party addons. This may take a moment.
- Navigate to the `/build` folder in Terminal and run `cmake -S .. -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -G Ninja`.
- You can run `ninja` in the `/build` folder to build the project, and the executable will be put in `/build/Debug`

#### Editing with Xcode
- Add a new directory called `.xcode` so that the file structure looks like this:
  ```
  Bakerman
        ├─ .xcode (The directory you just made)
        ├─ build
        ├─ game
        ├─ editor
        ╰─ engine
  ```
- Run `where ninja` in Terminal and note the output for later. This is the location of the Ninja build tool on your computer.
- Run `where cmake` in Terminal and note the output for later. This is the location of the CMake build system on your computer.
- In the `.xcode` directory, run `cmake -S .. -G Xcode` in Terminal.
- Open the `.xcodeproj` file in the `/.xcode` directory. It should open Xcode.
- Navigate to the project settings, the root object in the file tree.
- At the very bottom of the target list, press the add button to create a new build target.
- Select `Other -> External Build System`.
- Set the `Product Name` to `bakerman-xcode`.
- Set the `Build Tool` to Ninja's install location (`/opt/homebrew/bin/ninja` on my computer), and press Finish.
- Navigate to `bakerman-xcode` in the target list, delete the `Arguments` field, and set the `Directory` to the location of the `/build` directory.
- In the Xcode file navigator, select everything except the project root in the file tree, right click and select `delete`, and select `Remove references`.
- Select the `/engine`, `/editor`, `/game`, `/assets`, and `/CMakeLists.txt` files and directories Finder, drag them into the Xcode project under the root, select `reference in place`, and add them to the `bakerman-build` target. This allows Xcode to index the files properly.
- Click on the build scheme in Xcode's top center status bar and click on `Edit scheme`.
- Go to the build tab, select whatever target is there, and remove it. Then press the add button and add the `bakerman-xcode` target we just created.
- Press `cmd + B` in Xcode to build the target.
- Go back to the scheme editor in Xcode and in the run tab, change the executable to run, select other, and choose `Bakerman/build/Debug/Bakerman.app`.
- You should now be able to build with `cmd + B` and run and debug under LLDB in Xcode with `cmd + R`.
  - Xcode may present an error about BX_CONFIG_DEBUG not being defined. This is fine, delete or comment out the erroring if statement.
  - When building for release mode, you should rerun the CMake script from the command line with `Release` instead of `Debug` as the build type and use `ninja` directly on the command line. Make sure to rerun the CMake after with the `Debug` configuration.

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