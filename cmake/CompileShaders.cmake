# ╒═══════════════ CompileShaders.cmake ═╕
# │ Syngine                              │
# │ Created 2025-06-05                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2026     │
# │ Placeholder License                  │
# ╰──────────────────────────────────────╯
# root/engine/cmake/CompileShaders.cmake

# Function to automatically find and compile shaders using syntools CLI.
#
# Example Usage in main CMakeLists.txt:
#   list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#   include(${CMAKE_SOURCE_DIR}/engine/cmake/CompileShaders.cmake)
#
#   set(MY_SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders")
#   set(ALL_COMPILED_BINS)
#
#   compile_all_shaders(
#       SOURCE_DIRECTORY      ${MY_SHADER_SOURCE_DIR}
#       BGFX_SRC_INCLUDE_DIRS ${MY_BGFX_INCLUDE_DIR}
#       SHADER_FILES_OUTPUT_VAR ALL_COMPILED_BINS
#       # OPTIONAL SUFFIXES:
#       # VARYING_SUFFIX ".custom_vary.sc"
#       # VERTEX_SUFFIX ".custom_vert.sc"
#       # FRAGMENT_SUFFIX ".custom_frag.sc"
#   )
#
#   if(ALL_COMPILED_BINS)
#       add_custom_target(GameShaders ALL DEPENDS ${ALL_COMPILED_BINS})
#       # add_dependencies(ExecutableTarget GameShaders)
#   endif()
#

include(CMakeParseArguments)

function(compile_all_shaders)
    set(options "")
    set(oneValueArgs
        SOURCE_DIRECTORY            # Directory where original .sc files are located
        BGFX_SRC_INCLUDE_DIRS       # Include directory for bgfx (e.g., bgfx/src)
        SHADER_FILES_OUTPUT_VAR     # Variable name to store the list of all compiled .bin files
        VARYING_SUFFIX              # Suffix for varying definition files (e.g., .vary.sc)
        VERTEX_SUFFIX               # Suffix for vertex shader files (e.g., .vert.sc)
        FRAGMENT_SUFFIX             # Suffix for fragment shader files (e.g., .frag.sc)
    )
    set(multiValueArgs "") # No multi value arguments

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # --- Validate required arguments ---
    if(NOT ARG_SOURCE_DIRECTORY OR NOT IS_DIRECTORY "${ARG_SOURCE_DIRECTORY}")
        message(FATAL_ERROR "compile_all_shaders: SOURCE_DIRECTORY not provided or not a directory ('${ARG_SOURCE_DIRECTORY}').")
    endif()
    if(NOT ARG_BGFX_SRC_INCLUDE_DIRS)
        message(FATAL_ERROR "compile_all_shaders: BGFX_SRC_INCLUDE_DIRS not provided.")
    endif()
    if(NOT ARG_SHADER_FILES_OUTPUT_VAR)
        message(FATAL_ERROR "compile_all_shaders: SHADER_FILES_OUTPUT_VAR not provided.")
    endif()
    
    # --- Determine output directory based on platform ---
    if(APPLE)
        set(ARG_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/${EXECUTABLE_NAME}.app/Contents/Resources/shaders")
    else()
        set(ARG_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/shaders")
    endif()
    
    # --- Set defaults for suffixes if not provided ---
    set(resolved_varying_suffix  ".vary.sc")
    set(resolved_vertex_suffix   ".vert.sc")
    set(resolved_fragment_suffix ".frag.sc")

    if(DEFINED ARG_VARYING_SUFFIX  AND NOT "${ARG_VARYING_SUFFIX}" STREQUAL "")
        set(resolved_varying_suffix "${ARG_VARYING_SUFFIX}")
    endif()
    if(DEFINED ARG_VERTEX_SUFFIX   AND NOT "${ARG_VERTEX_SUFFIX}" STREQUAL "")
        set(resolved_vertex_suffix "${ARG_VERTEX_SUFFIX}")
    endif()
    if(DEFINED ARG_FRAGMENT_SUFFIX AND NOT "${ARG_FRAGMENT_SUFFIX}" STREQUAL "")
        set(resolved_fragment_suffix "${ARG_FRAGMENT_SUFFIX}")
    endif()

    # --- Find all fragment shader files ending in .sc ---
    file(GLOB_RECURSE ALL_FRAG_FILES
        RELATIVE "${ARG_SOURCE_DIRECTORY}"
        "${ARG_SOURCE_DIRECTORY}/*${resolved_fragment_suffix}"
    )

    set(compiled_shader_list_accumulator "")
    foreach(frag_shader_relative_path ${ALL_FRAG_FILES})
        # Skip files not ending in .sc
        if(NOT frag_shader_relative_path MATCHES "\\.sc$")
            continue()
        endif()
        
        get_filename_component(frag_filename ${frag_shader_relative_path} NAME)
        get_filename_component(shader_dir_relative ${frag_shader_relative_path} DIRECTORY)
        
        # Extract base name (e.g., "terrain" from "terrain.frag.sc")
        string(REPLACE "${resolved_fragment_suffix}" "" shader_base_name ${frag_filename})
        
        # Look for corresponding vertex shader
        set(vert_filename "${shader_base_name}${resolved_vertex_suffix}")
        set(vert_shader_path "${ARG_SOURCE_DIRECTORY}/${shader_dir_relative}/${vert_filename}")
        if(shader_dir_relative)
            set(vert_shader_path "${ARG_SOURCE_DIRECTORY}/${shader_dir_relative}/${vert_filename}")
        else()
            set(vert_shader_path "${ARG_SOURCE_DIRECTORY}/${vert_filename}")
        endif()
        
        if(NOT EXISTS "${vert_shader_path}")
            message(WARNING "No matching vertex shader found for ${frag_shader_relative_path}. Skipping.")
            continue()
        endif()

        # --- Attempt to find matching varying definition file ---
        set(expected_varying_file_name "${shader_base_name}${resolved_varying_suffix}")
        set(potential_varying_path "${ARG_SOURCE_DIRECTORY}/${shader_dir_relative}/varying/${expected_varying_file_name}")

        # --- Build output paths ---
        set(output_base_name "${shader_base_name}")
        if(shader_dir_relative)
            string(REPLACE "/" "_" output_base_name "${shader_dir_relative}_${shader_base_name}")
        endif()
        
        set(output_path_final "${ARG_OUTPUT_DIRECTORY}/${output_base_name}")
        
        # --- Build syntools command ---
        if(MSVC)
            set(syntools_cmd "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/syntools.exe" shader "${shader_base_name}" "s" "${output_path_final}" "--compiler=${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/shaderc.exe" "--src-ext=.sc")
        else()
            set(syntools_cmd "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/syntools" shader "${shader_base_name}" "s" "${output_path_final}" "--compiler=${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/shaderc" "--src-ext=.sc")
        endif()
        set(syntools_cmd ${syntools_cmd} "--src-dir=${ARG_SOURCE_DIRECTORY}")
        if(ARG_BGFX_SRC_INCLUDE_DIRS)
            set(syntools_cmd ${syntools_cmd} "--include=${ARG_BGFX_SRC_INCLUDE_DIRS}")
        endif()
        
        # --- Create custom command to compile shaders ---
        add_custom_command(
            OUTPUT ${output_path_final}.vert.bin ${output_path_final}.frag.bin
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ARG_OUTPUT_DIRECTORY}"
            COMMAND ${syntools_cmd}
            DEPENDS ${vert_shader_path} ${ARG_SOURCE_DIRECTORY}/${frag_shader_relative_path}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Compiling shader pair: ${shader_base_name}"
            VERBATIM
        )
        
        list(APPEND compiled_shader_list_accumulator ${output_path_final}.vert.bin ${output_path_final}.frag.bin)
    endforeach()
    # --- Set output variable with all compiled .bin files ---
    set(${ARG_SHADER_FILES_OUTPUT_VAR} ${compiled_shader_list_accumulator} PARENT_SCOPE)
endfunction()