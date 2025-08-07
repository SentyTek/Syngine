# ╒═════════════════════ CMakeLists.txt ═╕
# │ Syngine                              │
# │ Created YYYY-MM-DD                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2025     │
# │ Placeholder License                  │
# ╰──────────────────────────────────────╯
# Engine main CMakeLists (root/engine/CMakeLists.txt)

cmake_minimum_required(VERSION 3.16)
project(Syngine)
set(CMAKE_CXX_STANDARD 20)
set(SYNGINE_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE STRING "Syngine source dir" FORCE)

# Use static runtime on MSVC. Several libraries we use require this.
# Please don't change this.
if(MSVC)
    # For Debug configurations: Change /MDd to /MTd
    string(REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_DEBUG_TEMP "${CMAKE_C_FLAGS_DEBUG}")
    if(NOT CMAKE_C_FLAGS_DEBUG_TEMP MATCHES "/MTd") # Add /MTd if /MDd wasn't found (shouldn't happen for default debug)
        set(CMAKE_C_FLAGS_DEBUG_TEMP "${CMAKE_C_FLAGS_DEBUG_TEMP} /MTd")
    endif()
    string(REPLACE "/MD " " " CMAKE_C_FLAGS_DEBUG_TEMP "${CMAKE_C_FLAGS_DEBUG_TEMP}") # Remove /MD (release DLL) if present
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG_TEMP}" CACHE STRING "C Debug flags" FORCE)

    string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG_TEMP "${CMAKE_CXX_FLAGS_DEBUG}")
    if(NOT CMAKE_CXX_FLAGS_DEBUG_TEMP MATCHES "/MTd")
        set(CMAKE_CXX_FLAGS_DEBUG_TEMP "${CMAKE_CXX_FLAGS_DEBUG_TEMP} /MTd")
    endif()
    string(REPLACE "/MD " " " CMAKE_CXX_FLAGS_DEBUG_TEMP "${CMAKE_CXX_FLAGS_DEBUG_TEMP}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG_TEMP}" CACHE STRING "CXX Debug flags" FORCE)

    # For Release configurations: Change /MD to /MT
    string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE_TEMP "${CMAKE_C_FLAGS_RELEASE}")
    if(NOT CMAKE_C_FLAGS_RELEASE_TEMP MATCHES "/MT") # Add /MT if /MD wasn't found
        set(CMAKE_C_FLAGS_RELEASE_TEMP "${CMAKE_C_FLAGS_RELEASE_TEMP} /MT")
    endif()
    string(REPLACE "/MDd " " " CMAKE_C_FLAGS_RELEASE_TEMP "${CMAKE_C_FLAGS_RELEASE_TEMP}") # Remove /MDd (debug DLL) if present
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE_TEMP}" CACHE STRING "C Release flags" FORCE)

    string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE_TEMP "${CMAKE_CXX_FLAGS_RELEASE}")
    if(NOT CMAKE_CXX_FLAGS_RELEASE_TEMP MATCHES "/MT")
        set(CMAKE_CXX_FLAGS_RELEASE_TEMP "${CMAKE_CXX_FLAGS_RELEASE_TEMP} /MT")
    endif()
    string(REPLACE "/MDd " " " CMAKE_CXX_FLAGS_RELEASE_TEMP "${CMAKE_CXX_FLAGS_RELEASE_TEMP}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_TEMP}" CACHE STRING "CXX Release flags" FORCE)

    # Also set CMAKE_MSVC_RUNTIME_LIBRARY. Sub-projects might still query this variable.
    # This is a really ugly fix
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "$<$<CONFIG:Debug>:MultiThreadedDebug>$<$<CONFIG:Release>:MultiThreaded>$<$<CONFIG:RelWithDebInfo>:MultiThreaded>$<$<CONFIG:MinSizeRel>:MultiThreaded>"
        CACHE STRING "MSVC Runtime Library selection for Syngine project" FORCE
    )
endif()

# Add all required source files
add_library(Syngine STATIC
    src/Syngine/Core/Core.cpp
    src/Syngine/Core/Registry.cpp
    src/Syngine/Graphics/Renderer.cpp
    src/Syngine/Graphics/Windowing.cpp
    src/Syngine/Graphics/DebugRenderer.cpp
    src/Syngine/Graphics/TextureHelpers.cpp
    src/Syngine/Utils/ModelLoader.cpp
    src/Syngine/Utils/FsUtils.cpp
    src/Syngine/ECS/GameObject.cpp
    src/Syngine/Physics/Physics.cpp
    src/Syngine/Core/Logger.cpp
    src/Syngine/ECS/Components/MeshComponent.cpp
    src/Syngine/ECS/Components/TransformComponent.cpp
    src/Syngine/ECS/Components/RigidbodyComponent.cpp
    src/Syngine/ECS/Components/PlayerComponent.cpp
    src/Syngine/ECS/Components/CameraComponent.cpp
)

# Apple needs OBJC++ files for Metal support
if(APPLE)
    target_sources(Syngine PRIVATE
        src/Syngine/Graphics/MetalBridge.mm
    )
endif()

# --- Dependency Configuration ---
# Function to set a default option if not already defined
function(set_default_option var value docstring)
    if(NOT DEFINED ${var})
        set(${var} ${value} CACHE BOOL "${docstring}")
    endif()
endfunction()

# SDL3 Options
set_default_option(SDL_TEST OFF "Build SDL tests")
set_default_option(SDL_SHARED OFF "Build SDL as a shared library")
set_default_option(SDL_STATIC ON "Build SDL as a static library")
set_default_option(SDL_AUDIO_DISABLED ON "Disable SDL audio subsystem")
set_default_option(SDL_SENSOR_DISABLED OFF "Disable SDL sensor subsystem")
set_default_option(SDL_LOADSO_DISABLED ON "Disable SDL dynamic loading subsystem")

# Assimp Options
set_default_option(BUILD_SHARED_LIBS OFF "Build Assimp as a shared library")
set_default_option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF "Disable all exporters by default")
set_default_option(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF "Disable all importers by default")
set_default_option(ASSIMP_BUILD_ASSIMP_TOOLS OFF "Disable building Assimp tools")
set_default_option(ASSIMP_BUILD_TESTS OFF "Disable building Assimp tests")
set_default_option(ASSIMP_BUILD_GLTF_IMPORTER ON "Enable glTF importer")
set_default_option(ASSIMP_INSTALL OFF "Disable installing Assimp targets and config files")

# Jolt Physics Options
set_default_option(JOLT_BUILD_EXAMPLES OFF "Disable building Jolt examples")
set_default_option(JOLT_BUILD_TESTS OFF "Disable building Jolt tests")
set_default_option(JOLT_PROFILE_ENABLED OFF "Disable Jolt profiling")

# BGFX Options
set_default_option(BGFX_BUILD_TOOLS ON "Enable building BGFX tools")
set_default_option(BGFX_BUILD_EXAMPLES OFF "Disable building BGFX examples")
set(BGFX_BUILD_TOOLS_BINARIES shaderc ON CACHE BOOL "Enable building shaderc binary for shader compilation")

# Set platform specific options for BGFX and SDL
if(APPLE)
    set_default_option(BGFX_CONFIG_MULTITHREADED OFF "Disable multithreaded rendering for BGFX on Apple platforms")
    set_default_option(SDL_VIDEO_VULKAN OFF "Disable Vulkan support in SDL on Apple platforms")
    set_default_option(SDL_RENDER_DISABLED OFF "Enable SDL rendering subsystem on Apple platforms")

    set_default_option(BGFX_CONFIG_RENDERER_DIRECT3D12 OFF "Disable Direct3D 12 Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_DIRECT3D11 OFF "Disable Direct3D 11 Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_DIRECT3D9 OFF "Disable Direct3D 9 Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_OPENGL OFF "Disable OpenGL Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_VULKAN OFF "Disable Vulkan Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_METAL ON "Enable Metal Renderer")

    add_compile_options(-frtti) # Jolt needs RTTI on Apple platforms
elseif(MSVC)
    set_default_option(BGFX_CONFIG_RENDERER_DIRECT3D12 ON "Enable Direct3D 12 Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_DIRECT3D11 OFF "Disable Direct3D 11 Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_DIRECT3D9 OFF "Disable Direct3D 9 Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_OPENGL OFF "Disable OpenGL Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_VULKAN OFF "Disable Vulkan Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_METAL OFF "Disable Metal Renderer")
    set_default_option(SDL_RENDER_DISABLED ON "Disable SDL rendering subsystem on Windows platforms")

    set_default_option(ASSIMP_MSVC_STATIC_RUNTIME ON "Force Assimp to use static MSVC runtime (/MT or /MTd)")
else() # Linux and other Unix-like systems
    set_default_option(BGFX_CONFIG_RENDERER_DIRECT3D12 OFF "Disable Direct3D 12 Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_DIRECT3D11 OFF "Disable Direct3D 11 Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_DIRECT3D9 OFF "Disable Direct3D 9 Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_OPENGL OFF "Disable OpenGL Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_VULKAN ON "Enable Vulkan Renderer")
    set_default_option(BGFX_CONFIG_RENDERER_METAL OFF "Disable Metal Renderer")
    set_default_option(SDL_RENDER_DISABLED ON "Disable SDL rendering subsystem on Linux/Unix platforms")

    add_compile_options(-frtti) # Jolt needs RTTI on Unix platforms
endif()

# Configure all third party files by calling their cmake scripts
# EXCLUDE_FROM_ALL is used so that its install targets and configs don't pollute upwards into the parent project
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/SDL ${CMAKE_BINARY_DIR}/third_party/SDL EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/bgfx.cmake ${CMAKE_BINARY_DIR}/third_party/bgfx.cmake EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/assimp ${CMAKE_BINARY_DIR}/third_party/assimp EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/JoltPhysics/Build ${CMAKE_BINARY_DIR}/third_party/JoltPhysics EXCLUDE_FROM_ALL)

if(WIN32)
    # Since the root CMakeLists sets CMAKE_WIN32_EXECUTABLE, we need to ensure that
    # shaderc (a CLI tool) is built as a console application, not a GUI application.
    set_target_properties(shaderc PROPERTIES
        WIN32_EXECUTABLE OFF
    )
endif()

if(NOT TARGET shaderc)
    message(FATAL_ERROR "shaderc target not found. Ensure bgfx.cmake is properly included and configured.")
endif()

if(APPLE)
    set_target_properties(shaderc PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO") # Don't sign shaderc binary on macOS
endif()

if(MSVC)
    target_compile_options(Syngine PRIVATE
        /Zc:__cplusplus
        /Zc:preprocessor
    )
endif()

# Also ensure the BX_CONFIG_DEBUG preprocessor macro is set for Syngine
target_compile_definitions(Syngine PRIVATE
    "$<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>"
    "$<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>"
)

# --- Include Directories ---
target_include_directories(Syngine
    PUBLIC
        # Engine public headers directory
        ${CMAKE_CURRENT_LIST_DIR}/include
        
        # Third-party headers needed by engine's public headers
        ${CMAKE_CURRENT_LIST_DIR}/third_party/SDL/include
        ${CMAKE_CURRENT_LIST_DIR}/third_party/bgfx.cmake/bgfx/include
        ${CMAKE_CURRENT_LIST_DIR}/third_party/bgfx.cmake/bx/include   # Often needed by bgfx headers
        ${CMAKE_CURRENT_LIST_DIR}/third_party/bgfx.cmake/bimg/include # Often needed by bgfx headers
        ${CMAKE_CURRENT_LIST_DIR}/third_party/assimp/include
        ${CMAKE_CURRENT_LIST_DIR}/third_party/JoltPhysics
    PRIVATE
        # Engine private headers/sources directory
        ${CMAKE_CURRENT_LIST_DIR}/src

        ${CMAKE_CURRENT_LIST_DIR}/lib
)

# --- Link Libraries ---
# Link the targets created by add_subdirectory
target_link_libraries(Syngine PRIVATE
    SDL3::SDL3
    assimp::assimp
    bx
    bimg
    bgfx
    Jolt
)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/clangd.cmake)
clangd(Syngine
    EXCLUDE_PATHS "third_party/*"
    OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    INLAY_HINTS 
    INLAY_DEDUCED_TYPES 
    INLAY_PARAMETER_NAMES)
