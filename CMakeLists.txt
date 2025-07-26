cmake_minimum_required(VERSION 3.16)

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

project(Syngine)
set(CMAKE_CXX_STANDARD 20)

# create the executable
add_library(Syngine STATIC
    src/SyngineCore.cpp
    src/SyngineGraphics.cpp
    src/SynModelLoader.cpp
    src/helpers.cpp
    src/FsUtils.cpp
    src/SyngineGameobjects.cpp
    src/SynginePhys.cpp
    src/SyngineDebug.cpp
    src/SyngineLogger.cpp
    src/Components/MeshComponent.cpp
    src/Components/TransformComponent.cpp
    src/Components/RigidbodyComponent.cpp
    src/Components/PlayerComponent.cpp
    src/Components/CameraComponent.cpp
)

if(APPLE)
    target_sources(Syngine PRIVATE
        src/SynOSXMetalBridge.mm
        
        # get the headers to show up in Xcode
		${CMAKE_CURRENT_SOURCE_DIR}/src/Components/CameraComponent.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/Components/MeshComponent.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/Components/PlayerComponent.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/Components/RigidbodyComponent.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/Components/TransformComponent.h
    )
endif()

if(MSVC)
    set(ASSIMP_MSVC_STATIC_RUNTIME ON CACHE BOOL "Force Assimp to use static MSVC runtime (/MT or /MTd)" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_CONFIGURATION_TYPES MATCHES "Debug")
    set(LOCAL_BX_CONFIG_DEBUG ON)
else()
    set(LOCAL_BX_CONFIG_DEBUG OFF)
endif()
set(BX_CONFIG_DEBUG ${LOCAL_BX_CONFIG_DEBUG} CACHE BOOL "Enable BGFX debug mode" FORCE)
set(BX_CONFIG_DEBUG ${LOCAL_BX_CONFIG_DEBUG}) # Also set as normal variable

# set what gets built
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_AUDIO_DISABLED ON CACHE BOOL "" FORCE)
set(SDL_SENSOR_DISABLED OFF CACHE BOOL "" FORCE) # SDL sensor needs to be on idk
set(SDL_LOADSO_DISABLED ON CACHE BOOL "" FORCE)

if(APPLE)
set(SDL_RENDER_DISABLED OFF CACHE BOOL "" FORCE) # Apple requires it enabled
else()
set(SDL_RENDER_DISABLED ON CACHE BOOL "" FORCE)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # assimp
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE) # Enable glTF importer
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

set(JOLT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(JOLT_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(JOLT_PROFILE_ENABLED OFF CACHE BOOL "" FORCE)
if(UNIX)
add_compile_options(-frtti)
endif()

set(BGFX_BUILD_TOOLS ON CACHE BOOL "" FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS_BINARIES shaderc ON CACHE BOOL "" FORCE)

if(APPLE) # cocoa is really picky and requires entire renderer to be single threaded
    set(BGFX_CONFIG_MULTITHREADED OFF CACHE BOOL "" FORCE)
    set(SDL_VIDEO_VULKAN OFF CACHE BOOL "" FORCE)

    set(BGFX_CONFIG_RENDERER_DIRECT3D12 OFF CACHE BOOL "Disable Direct3D 12 Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_DIRECT3D11 OFF CACHE BOOL "Disable Direct3D 11 Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_DIRECT3D9 OFF CACHE BOOL "Disable Direct3D 9 Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_OPENGL OFF CACHE BOOL "Disable OpenGL Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_VULKAN OFF CACHE BOOL "Disable Vulkan Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_METAL ON CACHE BOOL "Enable Metal Renderer" FORCE)
endif()

if(MSVC)
    set(BGFX_CONFIG_RENDERER_DIRECT3D12 ON CACHE BOOL "Enable Direct3D 12 Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_DIRECT3D11 OFF CACHE BOOL "Disable Direct3D 11 Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_DIRECT3D9 OFF CACHE BOOL "Disable Direct3D 9 Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_OPENGL OFF CACHE BOOL "Disable OpenGL Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_VULKAN OFF CACHE BOOL "Disable Vulkan Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_METAL OFF CACHE BOOL "Disable Metal Renderer" FORCE)
endif()

if(UNIX AND NOT APPLE)
    set(BGFX_CONFIG_RENDERER_DIRECT3D12 OFF CACHE BOOL "Disable Direct3D 12 Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_DIRECT3D11 OFF CACHE BOOL "Disable Direct3D 11 Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_DIRECT3D9 OFF CACHE BOOL "Disable Direct3D 9 Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_OPENGL OFF CACHE BOOL "Disable OpenGL Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_VULKAN ON CACHE BOOL "Enable Vulkan Renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_METAL OFF CACHE BOOL "Disable Metal Renderer" FORCE)
endif()

# configure all third party files by calling their cmake scripts
# EXECLUDE_FROM_ALL is used so that its install targets and configs cont pollute upwards into the parent project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL ${CMAKE_BINARY_DIR}/third_party/SDL EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/bgfx.cmake ${CMAKE_BINARY_DIR}/third_party/bgfx.cmake EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp ${CMAKE_BINARY_DIR}/third_party/assimp EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/JoltPhysics/Build ${CMAKE_BINARY_DIR}/third_party/JoltPhysics EXCLUDE_FROM_ALL)

if(APPLE)
set_target_properties(shaderc PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO") # Don't sign shaderc binary on macOS
endif()

if(MSVC)
    target_compile_options(Syngine PRIVATE
        "$<$<CONFIG:Debug>:/MTd>"
        "$<$<NOT:$<CONFIG:Debug>>:/MT>" # For Release, RelWithDebInfo, MinSizeRel
    )

    # Also ensure the BX_CONFIG_DEBUG preprocessor macro is set for Syngine
    target_compile_definitions(Syngine PRIVATE
        "$<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>"
        "$<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>"
    )
endif()

# --- Include Directories ---
target_include_directories(Syngine
    PUBLIC
        # Engine public headers directory
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Components
        ${CMAKE_CURRENT_SOURCE_DIR}/lib

        # Third-party headers needed by engine's public headers
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bgfx.cmake/bgfx/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bgfx.cmake/bx/include   # Often needed by bgfx headers
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bgfx.cmake/bimg/include # Often needed by bgfx headers
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/JoltPhysics
    PRIVATE
        # Engine private headers/sources directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Components
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

clangdConfigure(Syngine)
