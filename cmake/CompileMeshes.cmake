# ╒════════════════ CompileMeshes.cmake ═╕
# │ Syngine                              │
# │ Created 2026-04-07                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2026     │
# │ Licensed under the MIT License       │
# ╰──────────────────────────────────────╯
# root/engine/cmake/CompileMeshes.cmake

include(CMakeParseArguments)

function(compile_all_meshes)
    set(options "")
    set(oneValueArgs
        SOURCE_DIRECTORY
        OUTPUT_DIRECTORY
        BUNDLE_FILES_OUTPUT_VAR
    )
    set(multiValueArgs "")

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARG_SOURCE_DIRECTORY)
        set(ARG_SOURCE_DIRECTORY "${CMAKE_SOURCE_DIR}/assets/meshes")
    endif()

    if(NOT IS_DIRECTORY "${ARG_SOURCE_DIRECTORY}")
        message(FATAL_ERROR "compile_all_meshes: SOURCE_DIRECTORY does not exist ('${ARG_SOURCE_DIRECTORY}').")
    endif()

    if(NOT ARG_OUTPUT_DIRECTORY)
        set(ARG_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/meshes")
    endif()

    if(NOT TARGET syntools)
        message(FATAL_ERROR "compile_all_meshes: 'syntools' target not found. Ensure it is built before this function is called.")
    endif()

    # Name-only glob: just enough to declare static bundle outputs for Ninja.
    # Actual per-bundle file discovery is delegated to `syntools pack-tree` at build time.
    file(GLOB mesh_top_entries RELATIVE "${ARG_SOURCE_DIRECTORY}" "${ARG_SOURCE_DIRECTORY}/*")

    set(generated_bundle_files "")
    set(has_loose_mesh_files FALSE)
    foreach(mesh_entry ${mesh_top_entries})
        if(IS_DIRECTORY "${ARG_SOURCE_DIRECTORY}/${mesh_entry}")
            list(APPEND generated_bundle_files "${ARG_OUTPUT_DIRECTORY}/${mesh_entry}.spk")
        else()
            set(has_loose_mesh_files TRUE)
        endif()
    endforeach()
    if(has_loose_mesh_files)
        list(APPEND generated_bundle_files "${ARG_OUTPUT_DIRECTORY}/meshes.spk")
    endif()

    if(generated_bundle_files)
        add_custom_command(
            OUTPUT ${generated_bundle_files}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ARG_OUTPUT_DIRECTORY}"
            COMMAND $<TARGET_FILE:syntools> pack-tree "${ARG_SOURCE_DIRECTORY}" "${ARG_OUTPUT_DIRECTORY}" "--root-bundle-name=meshes"
            DEPENDS syntools
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Bundling mesh tree from ${ARG_SOURCE_DIRECTORY}"
            VERBATIM
        )
    endif()

    if(ARG_BUNDLE_FILES_OUTPUT_VAR)
        set(${ARG_BUNDLE_FILES_OUTPUT_VAR} ${generated_bundle_files} PARENT_SCOPE)
    endif()
endfunction()
