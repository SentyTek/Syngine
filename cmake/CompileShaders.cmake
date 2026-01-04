# ╒═══════════════ CompileShaders.cmake ═╕
# │ Syngine                              │
# │ Created 2025-06-05                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2026     │
# │ Placeholder License                  │
# ╰──────────────────────────────────────╯
# root/engine/cmake/CompileShaders.cmake

# Function to automatically find and compile BGFX shaders from a directory.
#
# Example Usage in your main CMakeLists.txt:
#   list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake") # Or wherever this file is
#   include(${CMAKE_SOURCE_DIR}/engine/cmake/CompileShaders.cmake)
#
#   set(MY_SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders")
#   set(MY_SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/compiled_shaders")
#   set(MY_BGFX_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/bgfx.cmake/bgfx/src") # Adjust as needed
#   set(ALL_COMPILED_BINS)
#
#   compile_all_shaders(
#       SOURCE_DIRECTORY      ${MY_SHADER_SOURCE_DIR}
#       OUTPUT_DIRECTORY      ${MY_SHADER_OUTPUT_DIR}
#       BGFX_SRC_INCLUDE_DIRS ${MY_BGFX_INCLUDE_DIR}
#       SHADER_FILES_OUTPUT_VAR ALL_COMPILED_BINS
#       # OPTIONAL SUFFIXES:
#       # VARYING_SUFFIX ".custom_vary.sc"
#       # VERTEX_SUFFIX ".custom_vert.sc"
#       # FRAGMENT_SUFFIX ".custom_frag.sc"
#       # COMPUTE_SUFFIX ".custom_comp.sc"
#   )
#
#   if(ALL_COMPILED_BINS)
#       add_custom_target(GameShaders ALL DEPENDS ${ALL_COMPILED_BINS})
#       # Add dependency to your main target if needed
#       # add_dependencies(YourExecutableTarget GameShaders)
#   endif()
#

include(CMakeParseArguments)

function(compile_all_shaders)
    set(options "")
    set(oneValueArgs
        SOURCE_DIRECTORY            # Directory where original .sc files are located
        OUTPUT_DIRECTORY            # Root directory for compiled .bin shader files
        BGFX_SRC_INCLUDE_DIRS       # Include directory for bgfx (e.g., bgfx/src)
        SHADER_FILES_OUTPUT_VAR     # Variable name to store the list of all compiled .bin files
        VARYING_SUFFIX              # Suffix for varying definition files (e.g., .vary.sc)
        VERTEX_SUFFIX               # Suffix for vertex shader files (e.g., .vert.sc)
        FRAGMENT_SUFFIX             # Suffix for fragment shader files (e.g., .frag.sc)
        COMPUTE_SUFFIX              # Suffix for compute shader files (e.g., .comp.sc)
    )
    set(multiValueArgs "") # No multi value arguments

    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # --- Validate required arguments ---
    if(NOT ARG_SOURCE_DIRECTORY OR NOT IS_DIRECTORY "${ARG_SOURCE_DIRECTORY}")
        message(FATAL_ERROR "bgfx_discover_and_compile_shaders: SOURCE_DIRECTORY not provided or not a directory ('${ARG_SOURCE_DIRECTORY}').")
    endif()
    if(NOT ARG_OUTPUT_DIRECTORY)
        message(FATAL_ERROR "bgfx_discover_and_compile_shaders: OUTPUT_DIRECTORY not provided.")
    endif()
    if(NOT ARG_BGFX_SRC_INCLUDE_DIRS)
        message(FATAL_ERROR "bgfx_discover_and_compile_shaders: BGFX_SRC_INCLUDE_DIRS not provided.")
    endif()
    if(NOT ARG_SHADER_FILES_OUTPUT_VAR)
        message(FATAL_ERROR "bgfx_discover_and_compile_shaders: SHADER_FILES_OUTPUT_VAR not provided.")
    endif()

    # --- Set defaults for suffixes if not provided ---
    set(resolved_varying_suffix  ".vary.sc")
    set(resolved_vertex_suffix   ".vert.sc")
    set(resolved_fragment_suffix ".frag.sc")
    set(resolved_compute_suffix  ".comp.sc")

    if(DEFINED ARG_VARYING_SUFFIX  AND NOT "${ARG_VARYING_SUFFIX}" STREQUAL "")
        set(resolved_varying_suffix "${ARG_VARYING_SUFFIX}")
    endif()
    if(DEFINED ARG_VERTEX_SUFFIX   AND NOT "${ARG_VERTEX_SUFFIX}" STREQUAL "")
        set(resolved_vertex_suffix "${ARG_VERTEX_SUFFIX}")
    endif()
    if(DEFINED ARG_FRAGMENT_SUFFIX AND NOT "${ARG_FRAGMENT_SUFFIX}" STREQUAL "")
        set(resolved_fragment_suffix "${ARG_FRAGMENT_SUFFIX}")
    endif()
    if(DEFINED ARG_COMPUTE_SUFFIX  AND NOT "${ARG_COMPUTE_SUFFIX}" STREQUAL "")
        set(resolved_compute_suffix "${ARG_COMPUTE_SUFFIX}")
    endif()

    file(MAKE_DIRECTORY "${ARG_OUTPUT_DIRECTORY}")

    # --- Find all potential shader source files ---
    file(GLOB_RECURSE ALL_SC_FILES
        RELATIVE "${ARG_SOURCE_DIRECTORY}" # Get paths relative to the source directory
        "${ARG_SOURCE_DIRECTORY}/*${resolved_vertex_suffix}"
        "${ARG_SOURCE_DIRECTORY}/*${resolved_fragment_suffix}"
        "${ARG_SOURCE_DIRECTORY}/*${resolved_compute_suffix}"
    )

    set(compiled_shader_list_accumulator "") # To store all output .bin files

    foreach(shader_relative_path ${ALL_SC_FILES})
        get_filename_component(shader_filename ${shader_relative_path} NAME)      # e.g., terrain.vert.sc
        get_filename_component(shader_basename ${shader_filename} NAME_WE) # e.g., terrain.vert
        get_filename_component(shader_dir_relative ${shader_relative_path} DIRECTORY) # e.g., effects/ (or empty if in root)

        set(shader_absolute_path "${ARG_SOURCE_DIRECTORY}/${shader_relative_path}")
        set(current_shader_type "")
        set(varying_def_to_use "")

        # --- Determine shader type and expected varying definition file name ---
        string(REPLACE "${resolved_vertex_suffix}" "" common_name_part ${shader_filename})
        if(NOT "${common_name_part}" STREQUAL "${shader_filename}") # True if suffix was found and replaced
            set(current_shader_type "VERTEX")
        else()
            string(REPLACE "${resolved_fragment_suffix}" "" common_name_part ${shader_filename})
            if(NOT "${common_name_part}" STREQUAL "${shader_filename}")
                set(current_shader_type "FRAGMENT")
            else()
                string(REPLACE "${resolved_compute_suffix}" "" common_name_part ${shader_filename})
                if(NOT "${common_name_part}" STREQUAL "${shader_filename}")
                    set(current_shader_type "COMPUTE")
                else()
                    message(WARNING "Could not determine shader type for: ${shader_absolute_path}. Skipping.")
                    continue()
                endif()
            endif()
        endif()

        # --- Attempt to find matching varying definition file ---
        # e.g., if shader is "terrain.vert.sc", look for "terrain.vary.sc"
        set(expected_varying_file_name "${common_name_part}${resolved_varying_suffix}")
        set(potential_varying_path "${ARG_SOURCE_DIRECTORY}/${shader_dir_relative}varying/${expected_varying_file_name}")
        message(STATUS "Varying path: ${potential_varying_path} for shader: ${shader_absolute_path}")
        if(EXISTS "${potential_varying_path}")
            set(varying_def_to_use "${potential_varying_path}")
        else()
            # Scheme 2: A generic "varying.vary.sc" (or similar) in the same directory
            # This is a common fallback. You can customize this name.
            set(generic_varying_file_name "varying${resolved_varying_suffix}")
            set(potential_generic_varying_path "${ARG_SOURCE_DIRECTORY}/${shader_dir_relative}/${generic_varying_file_name}")
            if(EXISTS "${potential_generic_varying_path}")
                set(varying_def_to_use "${potential_generic_varying_path}")
            else()
                if(NOT "${current_shader_type}" STREQUAL "COMPUTE") # Compute shaders often don't need varying defs
                    message(STATUS "No specific ('${expected_varying_file_name}') or generic ('${generic_varying_file_name}') varying definition found for ${current_shader_type} shader: ${shader_absolute_path}. Compiling without explicit varying def.")
                else()
                     message(STATUS "No varying definition file found for compute shader ${shader_absolute_path} (as is often the case).")
                endif()
            endif()
        endif()

        # --- Determine output directory for this specific shader (maintaining subfolder structure) ---
        set(shader_specific_output_dir "${ARG_OUTPUT_DIRECTORY}")
        if(shader_dir_relative AND NOT "${shader_dir_relative}" STREQUAL ".")
            set(shader_specific_output_dir "${ARG_OUTPUT_DIRECTORY}/${shader_dir_relative}")
            file(MAKE_DIRECTORY "${shader_specific_output_dir}") # Ensure sub-output directory exists
        endif()

        set(current_shader_output_files "") # Variable to capture this specific compilation's output
        bgfx_compile_shaders(
            TYPE ${current_shader_type}
            SHADERS ${shader_absolute_path}
            VARYING_DEF ${varying_def_to_use} # Pass empty if not found; bgfx_compile_shaders handles it
            OUTPUT_DIR ${shader_specific_output_dir}
            INCLUDE_DIRS ${ARG_BGFX_SRC_INCLUDE_DIRS}
            OUT_FILES_VAR current_shader_output_files
        )

        if(current_shader_output_files)
            list(APPEND compiled_shader_list_accumulator ${current_shader_output_files})
            message(STATUS "  Successfully compiled. Output files: ${current_shader_output_files}")
        else()
            message(WARNING "  Compilation of ${shader_absolute_path} did not report output files. Check for errors from shaderc.")
        endif()
        message("") # Newline for readability

    endforeach()

    set(${ARG_SHADER_FILES_OUTPUT_VAR} "${compiled_shader_list_accumulator}" PARENT_SCOPE)
    message(STATUS "All discovered and compiled shader binary files: ${compiled_shader_list_accumulator}")
endfunction()