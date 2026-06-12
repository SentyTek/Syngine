# ╒═════════════════════ CMakeLists.txt ═╕
# │ Syngine                              │
# │ Created 2026-06-12                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2026     │
# | Licensed under the MIT License       |
# ╰──────────────────────────────────────╯

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(engine_tests
    ECS/AddRemoveComponents.cpp
)

if (MSVC)
    set_target_properties(engine_tests PROPERTIES WIN32_EXECUTABLE OFF)
    target_compile_options(engine_tests PRIVATE
        /Zc:__cplusplus
        /Zc:preprocessor
    )
endif()
target_compile_definitions(engine_tests PRIVATE
    "$<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>"       # Defines BX_CONFIG_DEBUG as 1 for Debug
    "$<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>" # Defines BX_CONFIG_DEBUG as 0 for others
)

set_target_properties(engine_tests PROPERTIES FOLDER "Tests")
set_target_properties(Catch2 PROPERTIES FOLDER "Tests")
set_target_properties(Catch2WithMain PROPERTIES FOLDER "Tests")
set_target_properties(engine_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/tests")

target_link_libraries(engine_tests PRIVATE
    Syngine
    Catch2::Catch2WithMain
)

include(CTest)

# Pull in the Catch module for discovery
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

# This will now properly register because CTest was initialized in the root
catch_discover_tests(engine_tests)
