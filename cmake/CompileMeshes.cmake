# ╒════════════════ CompileMeshes.cmake ═╕
# │ Syngine                              │
# │ Created 2026-04-07                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2026     │
# │ Licensed under the MIT License       │
# ╰──────────────────────────────────────╯
# root/engine/cmake/CompileMeshes.cmake

include(CMakeParseArguments)
include(${CMAKE_CURRENT_LIST_DIR}/FileBundle.cmake)

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

    set(generated_bundle_files "")

    # One bundle per immediate subdirectory in assets/meshes.
    file(GLOB mesh_entries RELATIVE "${ARG_SOURCE_DIRECTORY}" "${ARG_SOURCE_DIRECTORY}/*")

    set(root_mesh_files "")
    foreach(mesh_entry ${mesh_entries})
        set(full_path "${ARG_SOURCE_DIRECTORY}/${mesh_entry}")
        if(IS_DIRECTORY "${full_path}")
            create_file_bundle(
                BUNDLE_NAME "${mesh_entry}"
                OUTPUT_DIRECTORY "${ARG_OUTPUT_DIRECTORY}"
                SOURCE_DIRECTORY "${ARG_SOURCE_DIRECTORY}"
                INPUT_FILES "${mesh_entry}"
                BUNDLE_FILE_OUTPUT_VAR generated_bundle_file
            )
            list(APPEND generated_bundle_files "${generated_bundle_file}")
        else()
            list(APPEND root_mesh_files "${mesh_entry}")
        endif()
    endforeach()

    # Bundle loose files directly under assets/meshes.
    if(root_mesh_files)
        create_file_bundle(
            BUNDLE_NAME "meshes"
            OUTPUT_DIRECTORY "${ARG_OUTPUT_DIRECTORY}"
            SOURCE_DIRECTORY "${ARG_SOURCE_DIRECTORY}"
            INPUT_FILES ${root_mesh_files}
            BUNDLE_FILE_OUTPUT_VAR generated_root_bundle
        )
        list(APPEND generated_bundle_files "${generated_root_bundle}")
    endif()

    if(ARG_BUNDLE_FILES_OUTPUT_VAR)
        set(${ARG_BUNDLE_FILES_OUTPUT_VAR} ${generated_bundle_files} PARENT_SCOPE)
    endif()
endfunction()
