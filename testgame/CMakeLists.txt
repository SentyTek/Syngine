cmake_minimum_required(VERSION 3.10)
project(bakerman)
set(EXECUTABLE_NAME ${PROJECT_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
include(clangdConfig.cmake)
include(bgfxshaderc.cmake)

if(APPLE)
    # set the minimum deployment version for the asset compiler
    # there's nothing stopping us from going earlier than 26.0 Tahoe as far as I know
    set(MINIMUM_MACOS_VERSION 26.0)
endif()

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

if(UNIX AND NOT APPLE)
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local" CACHE INTERNAL "")
else()
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")
endif()

add_subdirectory(engine)

if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

add_executable(${EXECUTABLE_NAME}
    src/main.cpp
)

# Compile shaders
set(SHADER_SOURCE_DIR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR}) #ensure exists
set(BGFX_CORE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/engine/third_party/bgfx.cmake/bgfx/src")

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
    add_dependencies(${EXECUTABLE_NAME} Shaders)
else()
    message(STATUS "No shaders compiled.")
    add_custom_target(Shaders ALL)
endif()

set(SHADER_SOURCE_DIR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/engine/default/shaders")
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR}) #ensure exists
set(BGFX_CORE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/engine/third_party/bgfx.cmake/bgfx/src")

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
    add_dependencies(${EXECUTABLE_NAME} Shaders2)
else()
    message(STATUS "No shaders compiled.")
    add_custom_target(Shaders2 ALL)
endif()

# Include directories for the executable
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE Syngine)

# add platform specific flags
if(APPLE)
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE BX_PLATFORM_OSX=1)
elseif(UNIX AND NOT APPLE)
    # target_compile_definitions(${EXECUTABLE_NAME} PRIVATE BX_PLATFORM_LINUX=1)
endif()

if(MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE
        /Zc:__cplusplus
        /Zc:preprocessor
    )
    set(BX_COMPILE_DEBUG 1)
endif()

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
    "$<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>"       # Defines BX_CONFIG_DEBUG as 1 for Debug
    "$<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>" # Defines BX_CONFIG_DEBUG as 0 for others
)
message(STATUS "Root: Set BX_CONFIG_DEBUG preprocessor definition for ${EXECUTABLE_NAME}.")

clangdConfigure(${EXECUTABLE_NAME})

# dealing with assets
# if there's any non-code files, like images, audio, etc., we need to deal with per-platform
if(APPLE)
    # on macOS, we copy the assets into the app bundle's "Resouces" directory
    # SDL_GetBasePath will return the path to the app bundle, so we can use that to find the resources
    set(input_dir "${CMAKE_CURRENT_SOURCE_DIR}/src") # the directory where the assets are located
    macro(add_resource FILE)
        file(RELATIVE_PATH relpath "${input_dir}" "${FILE}")
        get_filename_component(relpath "${relpath}" DIRECTORY)
        target_sources(${EXECUTABLE_NAME} PRIVATE "${FILE}")
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
            target_sources(${EXECUTABLE_NAME} PRIVATE "${abs_platform_shader_file}")

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
            target_sources(${EXECUTABLE_NAME} PRIVATE "${abs_mesh_file}")
            set_property(SOURCE "${abs_mesh_file}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/meshes")
            message(STATUS "Bundling mesh for macOS: ${abs_mesh_file} -> Resources/meshes/${mesh_file_relative}")
        endforeach()
    else()
        message(WARNING "Meshes source directory not found: ${MESHES_SOURCE_DIR}")
    endif()

    # --- Copy engine/default subfolders except 'shaders' into bundle Resources ---
    set(ENGINE_DEFAULT_DIR "${CMAKE_SOURCE_DIR}/engine/default")
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
                    target_sources(${EXECUTABLE_NAME} PRIVATE "${abs_default_file}")
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
endif()

# --- Add the app icon ---
if(MSVC)
    # On Windows, we can use a resource file to set the icon
    target_sources(${EXECUTABLE_NAME} PRIVATE "src/res/app.rc")
elseif(APPLE)
    set(ICON_NAME ${EXECUTABLE_NAME}.icon)
    set(ICON_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/res/${ICON_NAME})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR/build/build/.${EXECUTABLE_NAME}icon})
    set(ICON_PACKAGE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/build/.${EXECUTABLE_NAME}icon)
    set(ICON_PACKAGE ${ICON_PACKAGE_PATH}/Assets.car)
    set(PLIST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/CMakeFiles/bakerman.dir/Info.plist)

    add_custom_command(
        OUTPUT "${ICON_PACKAGE}"
        COMMAND xcrun actool
            --compile ${ICON_PACKAGE_PATH}
            --app-icon ${ICON_PATH}
            --platform macosx
            --minimum-deployment-target ${MINIMUM_MACOS_VERSION}
            # add the new plist keys to CMake's existing generated plist
            --output-partial-info-plist ${PLIST_PATH}
            ${ICON_PATH}
        DEPENDS "${ICON_PATH}"
        COMMENT "Compiling app icon for macOS"
    )

    add_custom_target(${EXECUTABLE_NAME}-icon ALL
        DEPENDS "${ICON_PACKAGE}"
    )

    add_dependencies(${EXECUTABLE_NAME} ${EXECUTABLE_NAME}-icon)
    target_sources(${EXECUTABLE_NAME} PRIVATE ${ICON_PACKAGE})
    set_property(SOURCE ${ICON_PACKAGE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources")

    message(STATUS "Compiling icon for macOS ${MINIMUM_MACOS_VERSION}: ${ICON_PATH} -> Resources/Assets.car")
    message(STATUS "Added macOS icon property list")
else()
    set(DESKTOP_FILE "${CMAKE_BINARY_DIR}/${EXECUTABLE_NAME}.desktop")
    set(ICON_NAME "$ENV{HOME}/.local/share/pixmaps/${EXECUTABLE_NAME}.png")
    set(ICON_SOURCE "${CMAKE_SOURCE_DIR}/src/res/icon.png")
    set(ICON_TARGET "${CMAKE_BINARY_DIR}/${ICON_NAME}.png")
    set(EXEC_PATH "$ENV{HOME}/.local/bin/${EXECUTABLE_NAME}")

    # Configure .desktop file from template
    configure_file("${CMAKE_SOURCE_DIR}/src/res/${EXECUTABLE_NAME}.desktop.in"
                   "${DESKTOP_FILE}"
                   @ONLY)
    configure_file("${ICON_SOURCE}" "${ICON_TARGET}" COPYONLY)

    # Install the .desktop file and icon
    install(TARGETS ${EXECUTABLE_NAME}
        RUNTIME DESTINATION bin
        COMPONENT Runtime
    )
    install(FILES "${DESKTOP_FILE}" DESTINATION share/applications)
    install(FILES "${ICON_TARGET}" DESTINATION share/pixmaps)
endif()

# --- Target Properties (macOS Bundle Info) ---
if(APPLE)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/Info.plist.in
        XCODE_GENERATE_SCHEME TRUE
        XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.sentytek.bakerman"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.sentytek.bakerman"
        XCODE_ATTRIBUTE_SDKROOT "macosx"
        # tells Xcode to recognize the app icon
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "${EXECUTABLE_NAME}"
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_INCLUDE_ALL_APPICON_ASSETS TRUE
    )
endif()

#on macOS, we need to ensure the bundle is valid by calling fixup_bundle
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    install(TARGETS ${EXECUTABLE_NAME}
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

    set(BUNDLE_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/install/${EXECUTABLE_NAME}.app")
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

    if(EXISTS "${PLATFORM_SHADER_COPY_SOURCE_DIR}")
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${PLATFORM_SHADER_COPY_SOURCE_DIR}" # e.g., build/shaders_compiled/dx11
                "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/shaders" # e.g., build/Debug/shaders
            COMMENT "Copying ${SHADER_PLATFORM_SUBDIR_GENERIC} shaders to executable directory"
        )
    else()
        message(WARNING "Platform shader directory for copying not found: ${PLATFORM_SHADER_COPY_SOURCE_DIR}")
    endif()

    # Copy meshes
    if(EXISTS "${CMAKE_SOURCE_DIR}/meshes")
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/meshes"
                "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/meshes"
            COMMENT "Copying meshes to executable directory"
        )
    endif()

    # Copy engine defaults except the default/shaders folder
    if(EXISTS "${CMAKE_SOURCE_DIR}/engine/default")
        file(GLOB DEFAULT_SUBDIRS RELATIVE "${CMAKE_SOURCE_DIR}/engine/default" "${CMAKE_SOURCE_DIR}/engine/default/*")
        foreach(subdir ${DEFAULT_SUBDIRS})
            if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/engine/default/${subdir}" AND NOT "${subdir}" STREQUAL "shaders")
                add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${CMAKE_SOURCE_DIR}/engine/default/${subdir}"
                        "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/default/${subdir}"
                    COMMENT "Copying engine default subdir '${subdir}' to executable directory"
                )
            endif()
        endforeach()
    endif()
endif()
