# Adds the SyngineGame function, so it can set compile flags and definitions

include(${SYNGINE_SOURCE_DIR}/cmake/CompileShaders.cmake)

# Function to compile all shaders in a directory
function(compile_and_collect_shaders SHADER_SRC_DIR ALL_SHADERS_LIST)
    set(LOCAL_SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
    file(MAKE_DIRECTORY ${LOCAL_SHADER_OUTPUT_DIR}) # Ensure output directory exists
    set(LOCAL_BGFX_CORE_INCLUDE_DIR "${SYNGINE_SOURCE_DIR}/third_party/bgfx.cmake/bgfx/src")

    set(COMPILED_SHADER_BINARIES) # Temp list for this call
    compile_all_shaders(
        SOURCE_DIRECTORY ${SHADER_SRC_DIR}
        OUTPUT_DIRECTORY ${LOCAL_SHADER_OUTPUT_DIR}
        BGFX_SRC_INCLUDE_DIRS ${LOCAL_BGFX_CORE_INCLUDE_DIR}
        SHADER_FILES_OUTPUT_VAR COMPILED_SHADER_BINARIES
    )

    if(COMPILED_SHADER_BINARIES)
        message(STATUS "Found shaders to compile in ${SHADER_SRC_DIR}")
        # Append the new list to the main list in the parent scope
        list(APPEND ${ALL_SHADERS_LIST} ${COMPILED_SHADER_BINARIES})
        set(${ALL_SHADERS_LIST} ${${ALL_SHADERS_LIST}} PARENT_SCOPE)
    endif()
endfunction()

# Function to add assets to the target
function(add_assets target)
    if(WIN32 OR (UNIX AND NOT APPLE))
        set(SHADER_PLATFORM_SUBDIR "")
        if(WIN32)
            set(SHADER_PLATFORM_SUBDIR "dx11")
        else()
            set(SHADER_PLATFORM_SUBDIR "spirv")
        endif()

        set(PLATFORM_SHADER_SUBDIR "${CMAKE_BINARY_DIR}/shaders/${SHADER_PLATFORM_SUBDIR}")
        if(EXISTS "${PLATFORM_SHADER_SUBDIR}")
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${PLATFORM_SHADER_DIR}"
                    "$<TARGET_FILE_DIR:${target}>/shaders"
                COMMENT "Copying ${SHADER_PLATFORM_SUBDIR} shaders to executable directory"
            )
        endif()

        # Add other assets like meshes, icons, etc.
        if(EXISTS "${CMAKE_SOURCE_DIR}/meshes")
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${CMAKE_SOURCE_DIR}/meshes"
                    "$<TARGET_FILE_DIR:${target}>/meshes"
                COMMENT "Copying meshes to executable directory"
            )
        endif()
    elseif(APPLE)
        # macOS: Bundle resources into .app/Contents/Resources

        # to get an app icon on Apple, add it to the executable, then the image file in info.plist.in
        target_sources("${name}" PRIVATE "src/icon.png")

        # Helper macro to add a file to the bundle's Resources directory
        macro(add_resource FILE DEST_SUBDIR)
            get_filename_component(FILENAME "${FILE}" NAME)
            target_sources(${EXECUTABLE_NAME} PRIVATE "${FILE}")
            if(DEST_SUBDIR)
                set_property(SOURCE "${FILE}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${DEST_SUBDIR}")
            else()
                make_directory("${CMAKE_BINARY_DIR}/Resources/${DEST_SUBDIR}")
                set_property(SOURCE "${FILE}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${DEST_SUBDIR}")
            endif()
        endmacro()

        # Add shaders (metal)
        set(SHADER_PLATFORM_DIR "${SHADER_OUTPUT_DIR}/metal")
        if(EXISTS "${SHADER_PLATFORM_DIR}")
            file(GLOB_RECURSE SHADER_FILES RELATIVE "${SHADER_PLATFORM_DIR}" "${SHADER_PLATFORM_DIR}/*.bin")
            foreach(SHADER_FILE ${SHADER_FILES})
                set(ABS_SHADER_FILE "${SHADER_PLATFORM_DIR}/${SHADER_FILE}")
                if(NOT IS_DIRECTORY "${ABS_SHADER_FILE}")
                    get_filename_component(SUBDIR "${SHADER_FILE}" DIRECTORY)
                    add_resource("${ABS_SHADER_FILE}" "shaders/${SUBDIR}")
                endif()
            endforeach()
        else()
            message(WARNING "Shader output directory does not exist: ${SHADER_PLATFORM_DIR}")
        endif()

        # --- Add other assets like meshes ---
        set(MESHES_SOURCE_DIR "${CMAKE_SOURCE_DIR}/meshes")
        if(EXISTS "${MESHES_SOURCE_DIR}")
            file(GLOB_RECURSE MESH_FILES RELATIVE "${MESHES_SOURCE_DIR}" "${MESHES_SOURCE_DIR}/*")
            foreach(mesh_file_relative ${MESH_FILES})
                set(abs_mesh_file "${MESHES_SOURCE_DIR}/${mesh_file_relative}")
                if(IS_DIRECTORY "${abs_mesh_file}")
                    continue()
                endif()
                target_sources(${name} PRIVATE "${abs_mesh_file}")
                set_property(SOURCE "${abs_mesh_file}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/meshes")
                message(STATUS "Bundling mesh for macOS: ${abs_mesh_file} -> Resources/meshes/${mesh_file_relative}")
            endforeach()
        else()
            message(WARNING "Meshes source directory not found: ${MESHES_SOURCE_DIR}")
        endif()

        # --- Copy engine/default subfolders except 'shaders' into bundle Resources ---
        set(SYNGINE_DEFAULT_DIR "${CMAKE_SOURCE_DIR}/engine/default")
        if(EXISTS "${SYNGINE_DEFAULT_DIR}")
            file(GLOB ENGINE_DEFAULT_SUBDIRS RELATIVE "${SYNGINE_DEFAULT_DIR}" "${SYNGINE_DEFAULT_DIR}/*")
            foreach(subdir ${ENGINE_DEFAULT_SUBDIRS})
                if(IS_DIRECTORY "${SYNGINE_DEFAULT_DIR}/${subdir}" AND NOT "${subdir}" STREQUAL "shaders")
                    file(GLOB_RECURSE DEFAULT_SUBDIR_FILES RELATIVE "${SYNGINE_DEFAULT_DIR}/${subdir}" "${SYNGINE_DEFAULT_DIR}/${subdir}/*")
                    foreach(default_file_relative ${DEFAULT_SUBDIR_FILES})
                        set(abs_default_file "${SYNGINE_DEFAULT_DIR}/${subdir}/${default_file_relative}")
                        if(IS_DIRECTORY "${abs_default_file}")
                            continue()
                        endif()
                        target_sources(${EXECUTABLE_NAME} PRIVATE "${abs_default_file}")
                        set_property(SOURCE "${abs_default_file}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/default/${subdir}")
                        message(STATUS "Bundling engine default file for macOS: ${abs_default_file} -> Resources/default/${subdir}/${default_file_relative}")
                    endforeach()
                endif()
            endforeach()
        else()
            message(WARNING "Engine default directory not found: ${SYNGINE_DEFAULT_DIR}")
        endif()
    endif()
endfunction()

# Function to create a Syngine game executable target
function(SyngineGame name sources)
# Remove target name from argv
list(REMOVE_AT ARGV 0)

add_executable(${name} "${sources}")
target_link_libraries(${name} PRIVATE Syngine)

# Game's source dir is assumed to be its CMakeLists
set(${name}_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

# --- Shader compilation ---
# Collect and compile all shaders from game and engine
set(ALL_COMPILED_SHADER_BINARIES)
compile_and_collect_shaders("${CMAKE_CURRENT_SOURCE_DIR}/shaders" ALL_COMPILED_SHADER_BINARIES)
compile_and_collect_shaders("${CMAKE_CURRENT_SOURCE_DIR}/engine/default/shaders" ALL_COMPILED_SHADER_BINARIES)

# Create a single target for all shaders
if(ALL_COMPILED_SHADER_BINARIES)
    message(STATUS "Compiled shaders: ${ALL_COMPILED_SHADER_BINARIES}")
    add_custom_target(${name}_Shaders ALL DEPENDS ${ALL_COMPILED_SHADER_BINARIES})
    add_dependencies(${name} ${name}_Shaders)
else()
    message(STATUS "No shaders compiled.")
    add_custom_target(${name}_Shaders ALL)
endif()

# Add platform specific flags (this seems to cause issues on occasion, may be removed in future)
if(MSVC)
    target_compile_options(${name} PRIVATE
        "/Zc:__cplusplus"
        "/Zc:preprocessor"
    )
elseif(APPLE)
    target_compile_definitions(${name} PRIVATE BX_PLATFORM_OSX=1)
else()
    target_compile_definitions(${name} PRIVATE BX_PLATFORM_LINUX=1 BX_CONFIG_DEBUG=1)
endif()

target_compile_definitions(${name} PRIVATE
    "$<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>"
    "$<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>"
)

# --- Assets ---
# if there's any non-code files, like images, audio, etc., we need to deal with per-platform
# Important note is that if assets are added, CMake must be re-run to pick them up
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
add_assets(${EXECUTABLE_NAME})

# macOS bundle settings
set(RESOURCE_FILES_FOR_TARGET "src/icon.png")
if(APPLE)
set_target_properties(${name} PROPERTIES
    MACOSX_BUNDLE TRUE
    XCODE_GENERATE_SCHEME TRUE
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.sentytek.${name}"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.sentytek.${name}"
    RESOURCE "${RESOURCE_FILES_FOR_TARGET}"
)
endif()

# On macOS, we need to ensure the bundle is valid by calling fixup_bundle
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    install(TARGETS ${name}
        BUNDLE DESTINATION ./install COMPONENT Runtime
        RUNTIME DESTINATION Contents/MacOS
    )

    # Prepare search paths for fixup_bundle
    set(DEP_SEARCH_DIRS "${CMAKE_BINARY_DIR}")
    if(CMAKE_CONFIGURATION_TYPES) # Multi-config generator (e.g. Xcode, VS)
        foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
            list(APPEND DEP_SEARCH_DIRS "${CMAKE_BINARY_DIR}/${CONFIG}")
        endforeach()
    endif()
    string(REPLACE ";" ";;" DEP_DIR_FIXUP "${DEP_SEARCH_DIRS}") # fixup_bundle needs ;; for multiple paths on some CMakes

    # Bundle installation path
    set(BUNDLE_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/install/${name}.app")
    INSTALL(CODE
        "message(STATUS \"Fixing up bundle at: ${BUNDLE_INSTALL_PATH} with search paths: ${DEP_DIR_FIXUP}\")
         include(BundleUtilities)
         fixup_bundle(\"${BUNDLE_INSTALL_PATH}\" \"\" \"${DEP_DIR_FIXUP}\")"
        COMPONENT Runtime
    )

    # Packaging with CPack
    set(CPACK_GENERATOR "DragNDrop")
    include(CPack)
endif()

# Post-build asset copying for win/linux
if(WIN32 OR (UNIX AND NOT APPLE))
    set(SHADER_PLATFORM_SUBDIR_GENERIC "")
    if(WIN32)
        set(SHADER_PLATFORM_SUBDIR_GENERIC "dx11")
    else()
        set(SHADER_PLATFORM_SUBDIR_GENERIC "spirv")
    endif()

    set(PLATFORM_SHADER_COPY_SOURCE_DIR "${SHADER_OUTPUT_DIR}/${SHADER_PLATFORM_SUBDIR_GENERIC}")
    
    # Create output shader directory
    if (DEFINED PLATFORM_SHADER_COPY_SOURCE_DIR AND
        NOT EXISTS ${PLATFORM_SHADER_COPY_SOURCE_DIR})
        cmake_path(RELATIVE_PATH PLATFORM_SHADER_COPY_SOURCE_DIR
            BASE_DIRECTORY ${CMAKE_BINARY_DIR}
            OUTPUT_VARIABLE check_path
        )
        # If true, DO NOT create directory, and error
        if (NOT check_path STREQUAL "shaders/${SHADER_PLATFORM_SUBDIR_GENERIC}")
            message(FATAL_ERROR "Output shader path is invalid (${CMAKE_BINARY_DIR}/shaders/${SHADER_PLATFORM_SUBDIR_GENERIC})")
        endif()
        make_directory(${PLATFORM_SHADER_COPY_SOURCE_DIR})
        # Check if it was made
        if (NOT EXISTS ${PLATFORM_SHADER_COPY_SOURCE_DIR})
            message(FATAL_ERROR "Failed to create shader output directory")
        endif()
    endif()

    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PLATFORM_SHADER_COPY_SOURCE_DIR}" # e.g., build/shaders_compiled/dx11
            "$<TARGET_FILE_DIR:${name}>/shaders" # e.g., build/Debug/shaders
        COMMENT "Copying ${SHADER_PLATFORM_SUBDIR_GENERIC} shaders to executable directory"
    )

    # Copy meshes
    if(EXISTS "${CMAKE_SOURCE_DIR}/meshes")
        add_custom_command(TARGET ${name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/meshes"
                "$<TARGET_FILE_DIR:${name}>/meshes"
            COMMENT "Copying meshes to executable directory"
        )
    endif()

    # Copy icon
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/icon.png")
        add_custom_command(TARGET ${name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "${CMAKE_CURRENT_SOURCE_DIR}/src/icon.png"
                "$<TARGET_FILE_DIR:${name}>/icon.png"
            COMMENT "Copying icon.png to executable directory"
        )
    endif()

    # Copy engine defaults except the default/shaders folder
    if(EXISTS "${SYNGINE_SOURCE_DIR}/default")
        file(GLOB DEFAULT_SUBDIRS RELATIVE "${SYNGINE_SOURCE_DIR}/default" "${SYNGINE_SOURCE_DIR}/default/*")
        foreach(subdir ${DEFAULT_SUBDIRS})
            if(IS_DIRECTORY "${SYNGINE_SOURCE_DIR}/default/${subdir}" AND NOT "${subdir}" STREQUAL "shaders")
                add_custom_command(TARGET ${name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${SYNGINE_SOURCE_DIR}/default/${subdir}"
                        "$<TARGET_FILE_DIR:${name}>/default/${subdir}"
                    COMMENT "Copying engine default subdir '${subdir}' to executable directory"
                )
            endif()
        endforeach()
    endif()
endif()
endfunction(SyngineGame)