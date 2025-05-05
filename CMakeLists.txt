cmake_minimum_required(VERSION 3.16)
project(Syngine)

set(CMAKE_CXX_STANDARD 17)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

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

target_compile_features(Syngine PRIVATE cxx_std_20)

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
add_subdirectory(third_party/SDL EXCLUDE_FROM_ALL)
add_subdirectory(third_party/bgfx.cmake EXCLUDE_FROM_ALL)
add_subdirectory(third_party/assimp EXCLUDE_FROM_ALL)

# link the libs to the executable
target_link_libraries(Syngine PRIVATE #if using satellite libs, SDL must be linked LAST
    assimp::assimp
    bgfx
    bx
    bimg
    SDL3::SDL3
)

target_include_directories(Syngine PUBLIC 
    ${CMAKE_SOURCE_DIR}/src
    third_party/SDL/include
    third_party/bgfx.cmake/bgfx/include
    third_party/bgfx.cmake/bx/include
    third_party/bgfx.cmake/bimg/include
    third_party/assimp/include
)