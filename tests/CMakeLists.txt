# ╒═════════════════════ CMakeLists.txt ═╕
# │ Syngine                              │
# │ Created 2026-06-12                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2026     │
# | Licensed under the MIT License       |
# ╰──────────────────────────────────────╯

cmake_minimum_required(VERSION 3.31)

# Ensure CMP0091 is active for the test executable when run standalone
if (MSVC)
    cmake_policy(SET CMP0091 NEW)
    # Only override if the parent project hasn't already set it
    if (NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

project(SyngineTests)

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(engine_tests
    Core/StartStop.cpp
    ECS/GameObjectLifecycle.cpp
    ECS/TransformHierarchy.cpp
    ECS/EntityDeletion-Queries.cpp
    ECS/Zones.cpp
    Serialization/ObjectRoundTrip.cpp
    Physics/RigidbodyCreation.cpp
    Physics/PhysicsSync.cpp
    Lua/ScriptLoading.cpp
    Lua/Bindings.cpp
    Input/Binding.cpp
    Math/Vectors.cpp
    Math/Matrices.cpp
    Math/Quaternion.cpp
)

if (MSVC)
    set_target_properties(engine_tests PROPERTIES WIN32_EXECUTABLE OFF)
    target_compile_options(engine_tests PRIVATE
        /Zc:__cplusplus
        /Zc:preprocessor
    )
endif()
target_compile_definitions(engine_tests PRIVATE
    SYN_TEST_MODE=1
    "$<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>"       # Defines BX_CONFIG_DEBUG as 1 for Debug
    "$<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>" # Defines BX_CONFIG_DEBUG as 0 for others
)

# Core.cpp is compiled into the Syngine target, so test-mode defines must be
# applied there as well.
if(TARGET Syngine)
    target_compile_definitions(Syngine PRIVATE SYN_TEST_MODE=1)
endif()

# Tests that include <sol/sol.hpp> need direct access to Lua C headers.
target_include_directories(engine_tests PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../lib/lua
)

set_target_properties(engine_tests PROPERTIES FOLDER "Tests")
set_target_properties(Catch2 PROPERTIES FOLDER "Tests")
set_target_properties(Catch2WithMain PROPERTIES FOLDER "Tests")
set_target_properties(engine_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/tests")

# Apparently Catch2 needs to be linked first otherwise it and Syngine fight over main
# Entirely my fault but whatevs
target_link_libraries(engine_tests PRIVATE
    Catch2::Catch2WithMain
    Syngine
    lua
    sol2
)

include(CTest)

# Pull in the Catch module for discovery
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

catch_discover_tests(engine_tests)
