# Engine main CMakelists
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
    # The CACHE STRING FORCE makes it persistent for the Syngine project scope.
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "$<$<CONFIG:Debug>:MultiThreadedDebug>"
        "$<$<CONFIG:Release>:MultiThreaded>"
        "$<$<CONFIG:RelWithDebInfo>:MultiThreaded>"
        "$<$<CONFIG:MinSizeRel>:MultiThreaded>"
        CACHE STRING "MSVC Runtime Library selection for Syngine project" FORCE
    )
endif()

# Add all required source files.
add_library(Syngine STATIC
    src/SyngineCore.cpp
    src/SyngineGraphics.cpp
    src/SynModelLoader.cpp
    src/helpers.cpp
    src/SyngineGameobjects.cpp
    src/SynginePhys.cpp
    src/SyngineDebug.cpp
    src/Components/MeshComponent.cpp
    src/Components/TransformComponent.cpp
    src/Components/RigidbodyComponent.cpp
    src/Components/PlayerComponent.cpp
    src/Components/CameraComponent.cpp
)

# Apple needs Obj-C++ files for Metal rendering
if(APPLE)
    target_sources(Syngine PRIVATE
        src/SynOSXMetalBridge.mm
    )
endif()

# --- Dependency configuration ---
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
set_default_option(BGFX_BUILD_TOOLS_BINARIES shaderc ON "Enable building shaderc tool")

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

# configure all third party files by calling their cmake scripts
# EXCLUDE_FROM_ALL is used so that its install targets and configs cont pollute upwards into the parent project
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/SDL ${CMAKE_BINARY_DIR}/third_party/SDL EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/bgfx.cmake ${CMAKE_BINARY_DIR}/third_party/bgfx.cmake EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/assimp ${CMAKE_BINARY_DIR}/third_party/assimp EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/third_party/JoltPhysics/Build ${CMAKE_BINARY_DIR}/third_party/JoltPhysics EXCLUDE_FROM_ALL)

if(APPLE)
set_target_properties(shaderc PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO") # Don't sign shaderc binary on macOS
set(CMAKE_OSX_SYSROOT "macosx" CACHE STRING "macOS SDK root")
endif()

# Set config-specific preprocessor definitions for Syngine
target_compile_definitions(Syngine PRIVATE
    "$<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>"
    "$<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>"
)

if(MSVC)
    target_compile_options(Syngine PRIVATE
        /Zc:__cplusplus
        /Zc:preprocessor
    )
endif()

# --- Include Directories ---
target_include_directories(Syngine
    PUBLIC
        # Engine public headers directory
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${CMAKE_CURRENT_LIST_DIR}/include/Components
        ${CMAKE_CURRENT_LIST_DIR}/lib

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
        ${CMAKE_CURRENT_LIST_DIR}/src/Components
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

# If Syngine is the top-level project, add tests
if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/clangdConfig.cmake)

include(cmake/SyngineGame.cmake)
