cmake_minimum_required(VERSION 3.16)
project(Syngine)

set(CMAKE_CXX_STANDARD 17)

# create the executable
add_library(Syngine STATIC
    src/SyngineCore.cpp
    src/SyngineGraphics.cpp
    src/SynModelLoader.cpp
)

if(APPLE)
    target_sources(Syngine PRIVATE
        src/SynOSXMetalBridge.mm
    )
endif()

# set what gets built
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # assimp
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

set(BGFX_BUILD_TOOLS ON CACHE BOOL "" FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BX_CONFIG_DEBUG OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS_BINARIES shaderc ON CACHE BOOL "" FORCE)

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

# --- Include Directories ---
target_include_directories(Syngine
    PUBLIC
        # Engine public headers directory
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        # Third-party headers needed by engine's public headers (now relative to root)
        ${CMAKE_SOURCE_DIR}/third_party/SDL/include
        ${CMAKE_SOURCE_DIR}/third_party/bgfx.cmake/bgfx/include
        ${CMAKE_SOURCE_DIR}/third_party/bgfx.cmake/bx/include   # Often needed by bgfx headers
        ${CMAKE_SOURCE_DIR}/third_party/bgfx.cmake/bimg/include # Often needed by bgfx headers
        ${CMAKE_SOURCE_DIR}/third_party/assimp/include
    PRIVATE
        # Engine private headers/sources directory
        ${CMAKE_CURRENT_SOURCE_DIR}/src
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
)

clangdConfigure(Syngine)