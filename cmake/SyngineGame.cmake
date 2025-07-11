# Adds the SyngineGame function, so it can set compile flags and definitions

include(${SYNGINE_SOURCE_DIR}/cmake/bgfxshaderc.cmake)

function(SyngineGame name sources)
# Remove target name from argv
list(REMOVE_AT ARGV 0)

add_executable(${name} "${sources}")
target_link_libraries(${name} PRIVATE Syngine)

# Compile shaders
set(SHADER_SOURCE_DIR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR}) #ensure exists
set(BGFX_CORE_INCLUDE_DIR "${SYNGINE_SOURCE_DIR}/third_party/bgfx.cmake/bgfx/src")

set(ALL_COMPILED_SHADER_BINARIES)

compile_all_shaders(
    SOURCE_DIRECTORY      ${SHADER_SOURCE_DIR_ROOT}
    OUTPUT_DIRECTORY      ${SHADER_OUTPUT_DIR}
    BGFX_SRC_INCLUDE_DIRS ${BGFX_CORE_INCLUDE_DIR}
    SHADER_FILES_OUTPUT_VAR ALL_COMPILED_SHADER_BINARIES
)

if(ALL_COMPILED_SHADER_BINARIES)
    message(STATUS "Compiled shaders: ${ALL_COMPILED_SHADER_BINARIES}")
    add_custom_target(Shaders ALL DEPENDS ${ALL_COMPILED_SHADER_BINARIES})
    add_dependencies(${name} Shaders)
else()
    message(STATUS "No shaders compiled.")
    add_custom_target(Shaders ALL)
endif()

set(SHADER_SOURCE_DIR_ROOT "${SYNGINE_SOURCE_DIR}/default/shaders")
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR}) #ensure exists
set(BGFX_CORE_INCLUDE_DIR "${SYNGINE_SOURCE_DIR}/third_party/bgfx.cmake/bgfx/src")

set(ALL_COMPILED_SHADER_BINARIES)

compile_all_shaders(
    SOURCE_DIRECTORY      ${SHADER_SOURCE_DIR_ROOT}
    OUTPUT_DIRECTORY      ${SHADER_OUTPUT_DIR}
    BGFX_SRC_INCLUDE_DIRS ${BGFX_CORE_INCLUDE_DIR}
    SHADER_FILES_OUTPUT_VAR ALL_COMPILED_SHADER_BINARIES
)

if(ALL_COMPILED_SHADER_BINARIES)
    message(STATUS "Compiled shaders: ${ALL_COMPILED_SHADER_BINARIES}")
    add_custom_target(Shaders2 ALL DEPENDS ${ALL_COMPILED_SHADER_BINARIES})
    add_dependencies(${name} Shaders2)
else()
    message(STATUS "No shaders compiled.")
    add_custom_target(Shaders2 ALL)
endif()

# add platform specific flags
if(APPLE)
    target_compile_definitions(${name} PRIVATE BX_PLATFORM_OSX=1)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(${name} PRIVATE BX_PLATFORM_LINUX=1 BX_CONFIG_DEBUG=1)
endif()

if(MSVC)
    target_compile_options(${name} PRIVATE
        /Zc:__cplusplus
        /Zc:preprocessor
    )
    set(BX_COMPILE_DEBUG 1)
    target_compile_definitions(${name} PRIVATE
        "$<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>"       # Defines BX_CONFIG_DEBUG as 1 for Debug
        "$<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>" # Defines BX_CONFIG_DEBUG as 0 for others
    )
    message(STATUS "Root: Set BX_CONFIG_DEBUG preprocessor definition for ${name}.")
endif()

# to get an app icon on Apple, add it to the executable, then the image file in info.plist.in
if(APPLE)
    target_sources("${name}" PRIVATE "src/icon.png")
endif()



# dealing with assets
# if there's any non-code files, like images, audio, etc., we need to deal with per-platform
if(APPLE)
    # on macOS, we copy the assets into the app bundle's "Resouces" directory
    # SDL_GetBasePath will return the path to the app bundle, so we can use that to find the resources
    set(input_dir "${CMAKE_CURRENT_SOURCE_DIR}/src") # the directory where the assets are located
    macro(add_resource FILE)
        file(RELATIVE_PATH relpath "${input_dir}" "${FILE}")
        get_filename_component(relpath "${relpath}" DIRECTORY)
        target_sources(${name} PRIVATE "${FILE}")
        if(NOT relpath)
            set(relpath ".") # Default to current directory if relpath is empty
        endif()
        set_property(SOURCE "${FILE}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${relpath}")
    endmacro()
    # at this point we can add the resources but we aint got any

    # add shaders
    set(SHADER_PLATFORM_DIR "${SHADER_OUTPUT_DIR}/metal")
    if(EXISTS "${SHADER_PLATFORM_DIR}")
        file(GLOB_RECURSE PLATFORM_SHADER_FILES RELATIVE "${SHADER_PLATFORM_DIR}" "${SHADER_PLATFORM_DIR}/*.bin")
        foreach(platform_shader_file ${PLATFORM_SHADER_FILES})
            set(abs_platform_shader_file "${SHADER_PLATFORM_DIR}/${platform_shader_file}")
            if(IS_DIRECTORY ${abs_platform_shader_file})
                message(WARNING "Shader file is a directory: ${abs_platform_shader_file}")
                continue()
            endif()
            target_sources(${name} PRIVATE "${abs_platform_shader_file}")

            get_filename_component(shader_file_bundle_subdir ${platform_shader_file} DIRECTORY)
            if (shader_file_bundle_subdir AND NOT "${shader_file_bundle_subdir}" STREQUAL ".")
                 set_property(SOURCE "${abs_platform_shader_file}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/shaders/${shader_file_bundle_subdir}")
            else()
                 set_property(SOURCE "${abs_platform_shader_file}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/shaders")
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
    set(ENGINE_DEFAULT_DIR "${SYNGINE_SOURCE_DIR}/default")
    if(EXISTS "${ENGINE_DEFAULT_DIR}")
        file(GLOB ENGINE_DEFAULT_SUBDIRS RELATIVE "${ENGINE_DEFAULT_DIR}" "${ENGINE_DEFAULT_DIR}/*")
        foreach(subdir ${ENGINE_DEFAULT_SUBDIRS})
            if(IS_DIRECTORY "${ENGINE_DEFAULT_DIR}/${subdir}" AND NOT "${subdir}" STREQUAL "shaders")
                file(GLOB_RECURSE DEFAULT_SUBDIR_FILES RELATIVE "${ENGINE_DEFAULT_DIR}/${subdir}" "${ENGINE_DEFAULT_DIR}/${subdir}/*")
                foreach(default_file_relative ${DEFAULT_SUBDIR_FILES})
                    set(abs_default_file "${ENGINE_DEFAULT_DIR}/${subdir}/${default_file_relative}")
                    if(IS_DIRECTORY "${abs_default_file}")
                        continue()
                    endif()
                    target_sources(${name} PRIVATE "${abs_default_file}")
                    set_property(SOURCE "${abs_default_file}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/default/${subdir}")
                    message(STATUS "Bundling engine default file for macOS: ${abs_default_file} -> Resources/default/${subdir}/${default_file_relative}")
                endforeach()
            endif()
        endforeach()
    else()
        message(WARNING "Engine default directory not found: ${ENGINE_DEFAULT_DIR}")
    endif()

    # Set the location within the macOS bundle's Resources directory
    set_property(SOURCE ${COMPILED_SHADER_VS} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/shaders")
    set_property(SOURCE ${COMPILED_SHADER_FS} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/shaders")
    set_property(SOURCE ${CMAKE_SOURCE_DIR}/meshes PROPERTY MACOSX_PACKAGE_LOCATION "Resources/meshes")

    # --- Add other resources using your macro ---
    add_resource("${CMAKE_CURRENT_SOURCE_DIR}/src/icon.png")
endif()

# --- Target Properties (macOS Bundle Info) ---
set(RESOURCE_FILES_FOR_TARGET "src/icon.png")
if(APPLE)
set_target_properties(${name} PROPERTIES
    MACOSX_BUNDLE TRUE
    XCODE_GENERATE_SCHEME TRUE
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.sentytek.bakerman"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.sentytek.bakerman"
    RESOURCE "${RESOURCE_FILES_FOR_TARGET}"
    XCODE_ATTRIBUTE_SDKROOT "macosx"
)
endif()

#on macOS, we need to ensure the bundle is valid by calling fixup_bundle
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    install(TARGETS ${name}
        BUNDLE DESTINATION ./install COMPONENT Runtime
        RUNTIME DESTINATION Contents/MacOS
    )

    set(DEP_SEARCH_DIRS "${CMAKE_BINARY_DIR}")
    if(CMAKE_CONFIGURATION_TYPES) # Multi-config generator (e.g. Xcode, VS)
        foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
            list(APPEND DEP_SEARCH_DIRS "${CMAKE_BINARY_DIR}/${CONFIG}")
        endforeach()
    endif()
    string(REPLACE ";" ";;" DEP_DIR_FIXUP "${DEP_SEARCH_DIRS}") # fixup_bundle needs ;; for multiple paths on some CMakes

    set(BUNDLE_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/install/${name}.app")
    INSTALL(CODE
        "message(STATUS \"Fixing up bundle at: ${BUNDLE_INSTALL_PATH} with search paths: ${DEP_DIR_FIXUP}\")
         include(BundleUtilities)
         fixup_bundle(\"${BUNDLE_INSTALL_PATH}\" \"\" \"${DEP_DIR_FIXUP}\")"
        COMPONENT Runtime
    )

    set(CPACK_GENERATOR "DragNDrop")
    include(CPack)
endif()

#post build asset copying for win/linux
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
            OUTPUT_VARIABLE check_path)
        # If true, DO NOT create directory, and error
        if (NOT check_path STREQUAL "shaders/${SHADER_PLATFORM_SUBDIR_GENERIC}")
            message(FATAL_ERROR "Output shader path is invalid (${PLATFORM_SHADER_COPY_SOURCE_DIR})")
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