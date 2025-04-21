# Installation instructions for Syngine

## Dependencies 
### Windows
- Windows 10 22H2 or later
- Visual Studio 2022 with the Desktop C++ Kit installed
- Windows 10 SDK
- CMake 3.16 or later
- Ninja 1.12.1 or later

## Install
### Windows
- Clone or download the repository
- Open the Developer Command Prompt for VS 2022 from Windows Search or VsDevCmd.bat
- Navigate to the root folder of the repository
- Run the following commands:
  - `mkdir build`
  - `cd build`
  - `cmake .. -G "Ninja"`
  - `ninja bakerman`
- Once it's done building (it should only take a moment), you can open and run `bakerman.exe` in the build/Debug folder.
