# ╒═══════════════════ FileBundle.cmake ═╕
# │ Syngine                              │
# │ Created 2026-03-24                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2026     │
# │ Licensed under the MIT License       │
# ╰──────────────────────────────────────╯
# root/engine/cmake/FileBundle.cmake

include(CMakeParseArguments)

# Creates a custom command that packages files into a single syntools bundle.
#
# Required args:
#   BUNDLE_NAME              Name of the output bundle. ".spk" is appended if missing.
#   OUTPUT_DIRECTORY         Directory where the bundle file is written.
#   SOURCE_DIRECTORY         Base directory used with --src-dir for relative asset paths.
#   INPUT_FILES              Files/directories to pack (relative to SOURCE_DIRECTORY preferred).
#   BUNDLE_FILE_OUTPUT_VAR   Parent-scope variable receiving the output bundle path.
#
# Optional args:
#   DEPENDS                  Extra build dependencies for this bundle command.
function(create_file_bundle)
	set(options "")
	set(oneValueArgs
		BUNDLE_NAME
		OUTPUT_DIRECTORY
		SOURCE_DIRECTORY
		BUNDLE_FILE_OUTPUT_VAR
	)
	set(multiValueArgs
		INPUT_FILES
		DEPENDS
	)

	cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT ARG_BUNDLE_NAME)
		message(FATAL_ERROR "create_file_bundle: BUNDLE_NAME not provided.")
	endif()
	if(NOT ARG_OUTPUT_DIRECTORY)
		message(FATAL_ERROR "create_file_bundle: OUTPUT_DIRECTORY not provided.")
	endif()
	if(NOT ARG_SOURCE_DIRECTORY)
		message(FATAL_ERROR "create_file_bundle: SOURCE_DIRECTORY not provided.")
	endif()
	if(NOT ARG_INPUT_FILES)
		message(FATAL_ERROR "create_file_bundle: INPUT_FILES not provided.")
	endif()
	if(NOT ARG_BUNDLE_FILE_OUTPUT_VAR)
		message(FATAL_ERROR "create_file_bundle: BUNDLE_FILE_OUTPUT_VAR not provided.")
	endif()
	if(NOT TARGET syntools)
		message(FATAL_ERROR "create_file_bundle: 'syntools' target not found. Ensure it is built before this function is called.")
	endif()

	# Keep bundle naming consistent by always writing .spk bundles.
	set(bundle_file_name "${ARG_BUNDLE_NAME}")
	if(NOT bundle_file_name MATCHES "\\.spk$")
		set(bundle_file_name "${bundle_file_name}.spk")
	endif()

	set(bundle_output_path "${ARG_OUTPUT_DIRECTORY}/${bundle_file_name}")

	# Pack all provided files/dirs while preserving relative in-bundle paths via --src-dir.
	add_custom_command(
		OUTPUT "${bundle_output_path}"
		COMMAND ${CMAKE_COMMAND} -E make_directory "${ARG_OUTPUT_DIRECTORY}"
		COMMAND $<TARGET_FILE:syntools> pack "${bundle_output_path}" "--src-dir=${ARG_SOURCE_DIRECTORY}" ${ARG_INPUT_FILES}
		DEPENDS syntools ${ARG_DEPENDS}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMENT "Bundling assets into ${bundle_file_name}"
		VERBATIM
	)

	set(${ARG_BUNDLE_FILE_OUTPUT_VAR} "${bundle_output_path}" PARENT_SCOPE)
endfunction()
