#!/bin/sh
echo "Enter name of the project (no spaces): "
read PROJECT_NAME
echo "Creating new Syngine project '$PROJECT_NAME'. This may take a few moments..."

cd ../../
mkdir -p game
mkdir -p game/src
mkdir -p assets
mkdir -p assets/meshes
mkdir -p assets/shaders
mkdir -p assets/shaders/varying
mkdir -p system
mkdir -p editor
mkdir -p build

cd editor
git clone https://github.com/SentyTek/SyngineStudio.git .
cd ..

touch CMakeLists.txt
echo "# root/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(\"$PROJECT_NAME\")
set(EXECUTABLE_NAME \${PROJECT_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# Set the current year for copyright notices
set(CURRENT_YEAR 2025)

# Set the install prefix based on platform
if(UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_PREFIX \"\$ENV{HOME}/.local\" CACHE INTERNAL \"\")
else()
    set(CMAKE_INSTALL_PREFIX \"\${CMAKE_BINARY_DIR}\" CACHE INTERNAL \"\")
endif()

if(APPLE)
# set the minimum deployment version for the asset compiler
    set(MINIMUM_MACOS_VERSION 26.0)
    # set the bundle identifier for xcode
    set(BUNDLE_IDENTIFIER \"com.example.$PROJECT_NAME\")
endif()

# Enable making a Windows executable if on Windows
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY \"\${CMAKE_BINARY_DIR}/bin\")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY \"\${CMAKE_BINARY_DIR}/lib\")

# Add engine first
add_subdirectory(engine)

# Add the game
add_subdirectory(game)

# And finally the editor
# add_subdirectory(editor)" > CMakeLists.txt

cd game
touch CMakeLists.txt
echo '# CMakeLists.txt for the game (root/game/CMakeLists.txt)

set(GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(GAME_ASSET_DIR "${CMAKE_SOURCE_DIR}/assets")

include(${SYNGINE_SOURCE_DIR}/cmake/GameConfig.cmake)

# Invoke the engine to build the game
SyngineGame(${EXECUTABLE_NAME}
    SOURCES
        src/main.cpp
    SHADER_DIRS
        ${GAME_ASSET_DIR}/shaders
)
' > CMakeLists.txt

cd ../system
touch app.rc
echo "ID1_ICON ICON \"assets/icon.ico\"" > app.rc
touch $PROJECT_NAME.desktop.in
echo "[Desktop Entry]
Name=$PROJECT_NAME
Exec=@EXEC_PATH@
Icon=@ICON_NAME@
Type=Application
Categories=Game;" > $PROJECT_NAME.desktop.in
touch Info.plist.in
echo '<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>NSHighResolutionCapable</key>
	<true/>
	<key>CFBundleDevelopmentRegion</key>
	<string>en</string>
	<key>CFBundleExecutable</key>
	<string>${EXECUTABLE_NAME}</string>
	<key>CFBundleIdentifier</key>
	<string>${BUNDLE_IDENTIFIER}</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>1.0</string>
	<key>CFBundleName</key>
	<string>${EXECUTABLE_NAME}</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleShortVersionString</key>
	<string>1</string>
	<key>CFBundleVersion</key>
	<string>1</string>
	<key>NSHumanReadableCopyright</key>
	<string>Placeholder Copyright ${CURRENT_YEAR}. Please update this to your own project copyright</string>
</dict>
</plist>
' > info.plist.in
mkdir -p assets
cd ../
touch .gitignore
echo "build/
compile_commands.json
.cache/
.vscode/
.xcode/
*.xcodeproj
*.xcworkspace
.clangd
.DS_Store" > .gitignore

cd game/src
touch main.cpp
cat > main.cpp <<EOF

#include "Syngine/Syngine.h"
#include <string>
using namespace Syngine;

int AppMain(int argc, char* argv[]) {
    std::string gameName = "$PROJECT_NAME";
    Syngine::EngineConfig config   = { .windowTitle  = gameName,
                                       .windowWidth  = 1600,
                                       .windowHeight = 900,
                                       .usePhysics   = true };

    Syngine::RendererConfig rConfig  = { .useShadows      = true,
                                         .shadowDist      = 500.0f,
                                         .vsync           = true,
                                         .usePseudoCamera = false };

    Syngine::Logger::Init(gameName);
    Syngine::Logger::Log("Starting " + gameName);

    // Create game
    Syngine::Core engine(config);
    engine.Initialize(rConfig);

    // Create default camera
    Syngine::GameObject* camera = new Syngine::GameObject("MainCamera");
    Syngine::CameraComponent* cameraComp = camera->AddComponent<Syngine::CameraComponent>();


    Logger::Info("Starting event loop");
    while (engine.IsRunning()) {
        Profiler::Reset();
        {
            SYN_PROFILE_SCOPE("MainLoop")
            engine.HandleEvents();
            engine.Update();
            engine.Render(cameraComp);
        }
    }

    // Cleanup
    Syngine::Registry::Clear();
    Renderer::RemoveAllPrograms();
    Syngine::Logger::Shutdown();
    return 0;
}
EOF

cd ../../engine
cp .clang-format ../
cp .editorconfig ../
cp .pre-commit-config.yaml ../
cd ../
pip install pre-commit
pre-commit install

echo "\\033[0;32mSyngine project '$PROJECT_NAME' created successfully!\\033[0m"

if [ "$(uname -s)" == "Darwin" ]; then
    echo "\\033[0;33mPlease make sure to run \\033[1;33massets_xcassets.sh\\033[0;33m to create macOS asset placeholders\\033[0m"
fi
