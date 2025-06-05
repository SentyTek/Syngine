cmake_minimum_required(VERSION 3.16)

if(MSVC)
    message(STATUS "Engine (after project(Syngine)): Initial CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}")
    message(STATUS "Engine (after project(Syngine)): Initial CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
    message(STATUS "Engine (after project(Syngine)): Initial CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")
    message(STATUS "Engine (after project(Syngine)): Initial CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")

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
set(CMAKE_CXX_STANDARD 17)

# create the executable
add_library(Syngine STATIC
    src/SyngineCore.cpp
    src/SyngineGraphics.cpp
    src/SynModelLoader.cpp
    src/helpers.cpp
    src/SyngineGameobjects.cpp
    src/SynginePhys.cpp
    src/Components/MeshComponent.cpp
    src/Components/TransformComponent.cpp
    src/Components/PhysComponent.cpp
)

if(APPLE)
    target_sources(Syngine PRIVATE
        src/SynOSXMetalBridge.mm
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

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # assimp
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

set(BGFX_BUILD_TOOLS ON CACHE BOOL "" FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS_BINARIES shaderc ON CACHE BOOL "" FORCE)

# Set it as a CACHE variable for persistence and to influence BGFX's CMake scripts.
# The FORCE ensures it's set even if it was different in the cache.
set(BX_CONFIG_DEBUG ${LOCAL_BX_CONFIG_DEBUG} CACHE BOOL "Enable BGFX debug mode (controls BGFX library compilation)" FORCE)

# Also set it as a normal variable in the current scope *before* adding BGFX.
# This can sometimes help if BGFX checks for a non-cache variable directly.
set(BX_CONFIG_DEBUG ${LOCAL_BX_CONFIG_DEBUG})

if(APPLE) # cocoa is really picky and requires entire renderer to be single threaded
    set(BGFX_CONFIG_MULTITHREADED OFF CACHE BOOL "" FORCE)
    set(SDL_VIDEO_VULKAN OFF CACHE BOOL "" FORCE)
    set(SDL_VULKAN OFF CACHE BOOL "" FORCE)

    set(BGFX_CONFIG_RENDERER_METAL ON CACHE BOOL "Enable Metal renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_OPENGL OFF CACHE BOOL "Disable OpenGL renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_VULKAN OFF CACHE BOOL "Disable Vulkan renderer" FORCE)
endif()

# configure all third party files by calling their cmake scripts
# EXECLUDE_FROM_ALL is used so that its install targets and configs cont pollute upwards into the parent project
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/SDL ${CMAKE_BINARY_DIR}/third_party/SDL EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/bgfx.cmake ${CMAKE_BINARY_DIR}/third_party/bgfx.cmake EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/assimp ${CMAKE_BINARY_DIR}/third_party/assimp EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/JoltPhysics/build ${CMAKE_BINARY_DIR}/third_party/JoltPhysics EXCLUDE_FROM_ALL)

if(MSVC)
    target_compile_options(Syngine PRIVATE
        "$<$<CONFIG:Debug>:/MTd>"
        "$<$<NOT:$<CONFIG:Debug>>:/MT>" # For Release, RelWithDebInfo, MinSizeRel
    )
    message(STATUS "Syngine: Applied /MTd (Debug) and /MT (Release) explicit compile options to Syngine target.")

    # Also ensure the BX_CONFIG_DEBUG preprocessor macro is set for Syngine
    target_compile_definitions(Syngine PRIVATE
        "$<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>"
        "$<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>"
    )
    message(STATUS "Syngine: Set BX_CONFIG_DEBUG preprocessor definition for Syngine library.")
endif()

# --- Include Directories ---
target_include_directories(Syngine
    PUBLIC
        # Engine public headers directory
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Components
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
        # Third-party headers needed by engine's public headers (now relative to root)
        ${CMAKE_SOURCE_DIR}/third_party/SDL/include
        ${CMAKE_SOURCE_DIR}/third_party/bgfx.cmake/bgfx/include
        ${CMAKE_SOURCE_DIR}/third_party/bgfx.cmake/bx/include   # Often needed by bgfx headers
        ${CMAKE_SOURCE_DIR}/third_party/bgfx.cmake/bimg/include # Often needed by bgfx headers
        ${CMAKE_SOURCE_DIR}/third_party/assimp/include
        ${CMAKE_SOURCE_DIR}/third_party/JoltPhysics
    PRIVATE
        # Engine private headers/sources directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Components
        # Add any third-party includes ONLY needed by engine's private .cpp files (if any)
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