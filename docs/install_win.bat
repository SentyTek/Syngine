@echo off
setlocal enabledelayedexpansion

set /p PROJECT_NAME="Enter name of the project (no spaces): "
echo Creating new Syngine project: %PROJECT_NAME%. This may take a few moments...

cd ..\..\
if not exist game mkdir game
if not exist game\src mkdir game\src
if not exist assets mkdir assets
if not exist assets\meshes mkdir assets\meshes
if not exist assets\shaders mkdir assets\shaders
if not exist assets\shaders\varying mkdir assets\shaders\varying
if not exist system mkdir system
if not exist editor mkdir editor
if not exist build mkdir build

cd editor
git clone https://github.com/SentyTek/SyngineStudio.git .
cd ..

(
echo # root/CMakeLists.txt
echo cmake_minimum_required^(VERSION 3.10^)
echo project^("%PROJECT_NAME%"^)
echo set^(EXECUTABLE_NAME ${PROJECT_NAME}^)
echo set^(CMAKE_EXPORT_COMPILE_COMMANDS ON^)
echo set^(CMAKE_CXX_STANDARD 20^)
echo.
echo # Set the current year for copyright notices
echo set^(CURRENT_YEAR 2025^)
echo.
echo # Set the install prefix based on platform
echo if^(UNIX AND NOT APPLE^)
echo     set^(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local" CACHE INTERNAL ""^)
echo else^(^)
echo     set^(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL ""^)
echo endif^(^)
echo.
echo if^(APPLE^)
echo # set the minimum deployment version for the asset compiler
echo     set^(MINIMUM_MACOS_VERSION 26.0^)
echo     # set the bundle identifier for xcode
echo     set^(BUNDLE_IDENTIFIER "com.example.%PROJECT_NAME%"^)
echo endif^(^)
echo.
echo # Enable making a Windows executable if on Windows
echo if^(WIN32^)
echo     set^(CMAKE_WIN32_EXECUTABLE ON^)
echo endif^(^)
echo.
echo # set the output directory for built objects.
echo # This makes sure that the dynamic library goes into the build directory automatically.
echo set^(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$^<CONFIGURATION^>"^)
echo set^(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$^<CONFIGURATION^>"^)
echo.
echo # Add engine first
echo add_subdirectory^(engine^)
echo.
echo # Add the game
echo add_subdirectory^(game^)
echo.
echo # And finally the editor
echo # add_subdirectory^(editor^)
) > CMakeLists.txt

cd game
(
echo # CMakeLists.txt for the game ^(root/game/CMakeLists.txt^)
echo.
echo set^(GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}"^)
echo set^(GAME_ASSET_DIR "${CMAKE_SOURCE_DIR}/assets"^)
echo.
echo include^(${SYNGINE_SOURCE_DIR}/cmake/GameConfig.cmake^)
echo.
echo # Invoke the engine to build the game
echo SyngineGame^(${EXECUTABLE_NAME}
echo     SOURCES
echo         src/main.cpp
echo     SHADER_DIRS
echo         ${GAME_ASSET_DIR}/shaders
echo ^)
) > CMakeLists.txt

cd ..\system
(
echo ID1_ICON ICON "assets/icon.ico"
) > app.rc

(
echo [Desktop Entry]
echo Name=%PROJECT_NAME%
echo Exec=@EXEC_PATH@
echo Icon=@ICON_NAME@
echo Type=Application
echo Categories=Game;
) > %PROJECT_NAME%.desktop.in

(
echo ^<?xml version="1.0" encoding="UTF-8"?^>
echo ^<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd"^>
echo ^<plist version="1.0"^>
echo ^<dict^>
echo 	^<key^>NSHighResolutionCapable^</key^>
echo 	^<true/^>
echo 	^<key^>CFBundleDevelopmentRegion^</key^>
echo 	^<string^>en^</string^>
echo 	^<key^>CFBundleExecutable^</key^>
echo 	^<string^>${EXECUTABLE_NAME}^</string^>
echo 	^<key^>CFBundleIdentifier^</key^>
echo 	^<string^>${BUNDLE_IDENTIFIER}^</string^>
echo 	^<key^>CFBundleInfoDictionaryVersion^</key^>
echo 	^<string^>1.0^</string^>
echo 	^<key^>CFBundleName^</key^>
echo 	^<string^>${EXECUTABLE_NAME}^</string^>
echo 	^<key^>CFBundlePackageType^</key^>
echo 	^<string^>APPL^</string^>
echo 	^<key^>CFBundleShortVersionString^</key^>
echo 	^<string^>1^</string^>
echo 	^<key^>CFBundleVersion^</key^>
echo 	^<string^>1^</string^>
echo 	^<key^>NSHumanReadableCopyright^</key^>
echo 	^<string^>Copyright 2025-${CURRENT_YEAR} SentyTek. All rights reserved.^</string^>
echo ^</dict^>
echo ^</plist^>
) > info.plist.in

if not exist assets mkdir assets
cd ..

(
echo build/
echo compile_commands.json
echo .cache/
echo .vscode/
echo .xcode/
echo *.xcodeproj
echo *.xcworkspace
echo .clangd
echo .DS_Store
) > .gitignore

cd game\src
(
echo #include "Syngine/Syngine.h"
echo #include ^<string^>
echo using namespace Syngine;
echo.
echo int AppMain^(int argc, char* argv[]^) {
echo     // Create engine config
echo     std::string gameName = "%PROJECT_NAME%";
echo     Syngine::EngineConfig config = {
echo         .windowTitle = gameName,
echo         .windowWidth = 1600,
echo         .windowHeight = 900,
echo         .vsync = true
echo     };
echo.
echo     // Initialize logger
echo     Syngine::Logger::Init^(gameName^);
echo     Syngine::Logger::Log^("Starting " + gameName^);
echo.
echo     // Create game
echo     Syngine::Core engine^(config^);
echo     engine.Initialize^(^);
echo.
echo     // Create default camera
echo     Syngine::GameObject* camera = new Syngine::GameObject^("MainCamera"^);
echo     Syngine::CameraComponent* cameraComp = camera-^>AddComponent^<Syngine::CameraComponent^>^(^);
echo.
echo     Logger::Info^("Starting event loop"^);
echo     while ^(engine.IsRunning^(^)^) {
echo         engine.HandleEvents^(^);
echo         engine.Update^(^);
echo         engine.Render^(cameraComp^);
echo     }
echo.
echo     // Cleanup
echo     Syngine::Registry::Clear^(^);
echo     Renderer::RemoveAllPrograms^(^);
echo     Syngine::Logger::Shutdown^(^);
echo     return 0;
echo }
) > main.cpp

cd ..\..

echo Syngine project '%PROJECT_NAME%' created successfully!
endlocal