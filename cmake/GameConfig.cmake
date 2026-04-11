# ╒═══════════════════ GameConfig.cmake ═╕
# │ Syngine                              │
# │ Created 2025-08-05                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2026     │
# │ Licensed under the MIT License       │
# ╰──────────────────────────────────────╯
# Adds several functions to compile shaders, add assets, and configure the game target (root/engine/cmake/GameConfig.cmake)

include(${SYNGINE_SOURCE_DIR}/cmake/CompileShaders.cmake)
include(${SYNGINE_SOURCE_DIR}/cmake/CompileMeshes.cmake)
include(${SYNGINE_SOURCE_DIR}/cmake/FileBundle.cmake)

# Function to compile all shaders in a directory
function(compile_collect_shaders SHADER_SRC_DIR ALL_SHADERS_LIST)
    set(LOCAL_SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
    set(LOCAL_SHADER_BUNDLE_DIR "${CMAKE_BINARY_DIR}/$<CONFIG>/rom/shaders")
    file(MAKE_DIRECTORY ${LOCAL_SHADER_OUTPUT_DIR}) # Ensure output directory exists
    set(LOCAL_BGFX_CORE_INCLUDE_DIR "${SYNGINE_SOURCE_DIR}/third_party/bgfx.cmake/bgfx/src")

    set(COMPILED_SHADER_BINARIES) # Temp list for this call
    set(BUNDLED_SHADER_PACKS)     # Temp list for this call

    if(SHADER_SRC_DIR STREQUAL "${SYNGINE_SOURCE_DIR}/default/shaders")
        compile_all_shaders(
            SOURCE_DIRECTORY ${SHADER_SRC_DIR}
            OUTPUT_DIRECTORY ${LOCAL_SHADER_OUTPUT_DIR}
            BGFX_SRC_INCLUDE_DIRS ${LOCAL_BGFX_CORE_INCLUDE_DIR}
            SHADER_FILES_OUTPUT_VAR COMPILED_SHADER_BINARIES
            BUNDLE_OUTPUT_DIRECTORY ${LOCAL_SHADER_BUNDLE_DIR}
            SINGLE_BUNDLE_NAME default_shaders
            BUNDLE_FILES_OUTPUT_VAR BUNDLED_SHADER_PACKS
        )
    else()
        compile_all_shaders(
            SOURCE_DIRECTORY ${SHADER_SRC_DIR}
            OUTPUT_DIRECTORY ${LOCAL_SHADER_OUTPUT_DIR}
            BGFX_SRC_INCLUDE_DIRS ${LOCAL_BGFX_CORE_INCLUDE_DIR}
            SHADER_FILES_OUTPUT_VAR COMPILED_SHADER_BINARIES
            BUNDLE_OUTPUT_DIRECTORY ${LOCAL_SHADER_BUNDLE_DIR}
            BUNDLE_BY_TOP_LEVEL_DIR
            BUNDLE_FILES_OUTPUT_VAR BUNDLED_SHADER_PACKS
        )
    endif()

    if(BUNDLED_SHADER_PACKS)
        message(STATUS "Found shaders to compile in ${SHADER_SRC_DIR}")
        # Depend on bundles; they already depend on compiled shader binaries.
        list(APPEND ${ALL_SHADERS_LIST} ${BUNDLED_SHADER_PACKS})
        set(${ALL_SHADERS_LIST} "${${ALL_SHADERS_LIST}};${BUNDLED_SHADER_PACKS}" PARENT_SCOPE)
    endif()
endfunction()

function(compile_collect_default_gizmos GIZMO_BUNDLE_OUTPUT_VAR)
    set(DEFAULT_GIZMO_SOURCE_DIR "${SYNGINE_SOURCE_DIR}/default/gizmos")

    if(NOT IS_DIRECTORY "${DEFAULT_GIZMO_SOURCE_DIR}")
        set(${GIZMO_BUNDLE_OUTPUT_VAR} "" PARENT_SCOPE)
        return()
    endif()

    file(GLOB DEFAULT_GIZMO_ENTRIES RELATIVE "${DEFAULT_GIZMO_SOURCE_DIR}" "${DEFAULT_GIZMO_SOURCE_DIR}/*")
    if(NOT DEFAULT_GIZMO_ENTRIES)
        set(${GIZMO_BUNDLE_OUTPUT_VAR} "" PARENT_SCOPE)
        return()
    endif()

    create_file_bundle(
        BUNDLE_NAME "default_gizmos"
        OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/rom/gizmos"
        SOURCE_DIRECTORY "${DEFAULT_GIZMO_SOURCE_DIR}"
        INPUT_FILES ${DEFAULT_GIZMO_ENTRIES}
        BUNDLE_FILE_OUTPUT_VAR GENERATED_GIZMO_BUNDLE
    )

    set(${GIZMO_BUNDLE_OUTPUT_VAR} "${GENERATED_GIZMO_BUNDLE}" PARENT_SCOPE)
endfunction()

function(compile_collect_asset_bundle ASSET_SRC_DIR BUNDLE_NAME ASSET_BUNDLE_OUTPUT_VAR)
    if(NOT IS_DIRECTORY "${ASSET_SRC_DIR}")
        set(${ASSET_BUNDLE_OUTPUT_VAR} "" PARENT_SCOPE)
        return()
    endif()

    file(GLOB ASSET_ENTRIES RELATIVE "${ASSET_SRC_DIR}" "${ASSET_SRC_DIR}/*")
    if(NOT ASSET_ENTRIES)
        set(${ASSET_BUNDLE_OUTPUT_VAR} "" PARENT_SCOPE)
        return()
    endif()

    create_file_bundle(
        BUNDLE_NAME "${BUNDLE_NAME}"
        OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/rom/${BUNDLE_NAME}"
        SOURCE_DIRECTORY "${ASSET_SRC_DIR}"
        INPUT_FILES ${ASSET_ENTRIES}
        BUNDLE_FILE_OUTPUT_VAR GENERATED_ASSET_BUNDLE
    )

    set(${ASSET_BUNDLE_OUTPUT_VAR} "${GENERATED_ASSET_BUNDLE}" PARENT_SCOPE)
endfunction()

# Asset handling helpers
function(_add_assets_win_linux target)
    set(ROM_DIR "$<TARGET_FILE_DIR:${target}>/rom")

    get_target_property(COPY_ROM ${target} SYNGINE_COPY_ROM)
    if(COPY_ROM)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ROM_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_BINARY_DIR}/$<CONFIG>/rom"
                "${ROM_DIR}"
            COMMENT "Copying staged rom assets to executable directory"
        )
    endif()
endfunction()

function(_add_assets_mac target)
    set(ROM_DIR "$<TARGET_BUNDLE_CONTENT_DIR:${target}>/Resources/rom")

    get_target_property(COPY_ROM ${target} SYNGINE_COPY_ROM)
    if(COPY_ROM)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ROM_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_BINARY_DIR}/$<CONFIG>/rom"
                "${ROM_DIR}"
            COMMENT "Copying staged rom assets into app Resources"
        )
    endif()
endfunction()

# Function to add assets to the target
function(add_assets target)
    if(WIN32 OR (UNIX AND NOT APPLE))
        _add_assets_win_linux(${target})
    elseif(APPLE)
        _add_assets_mac(${target})
    else()
        message(FATAL_ERROR "Unsupported platform for add_assets function (How)")
    endif()

    # --- Add the app icon ---
    if(MSVC)
        # on Windows, we can use a resource file to set the icon
        target_sources(${target} PRIVATE "${CMAKE_SOURCE_DIR}/system/app.rc")
    elseif(APPLE)
        # find the source icon path
        set(ICON_PATH "${CMAKE_SOURCE_DIR}/system/assets/${target}.icon")
        # find the asset bundle path
        set(ASSET_BUNDLE ${CMAKE_SOURCE_DIR}/system/assets/Assets.xcassets)
        # ensure the build cache directory for the icon exists
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/macOS)
        # find the icon build cache we just made, as well as the compiled icon
        set(MAC_CACHE_PATH ${CMAKE_BINARY_DIR}/macOS)
        file(MAKE_DIRECTORY "${MAC_CACHE_PATH}")

        set(ASSET_PACKAGE ${MAC_CACHE_PATH}/Assets.car)
        # path to the generated-info.plist file in the build cache
        set(GENERATED_PLIST_PATH ${MAC_CACHE_PATH}/generated-info.plist)
        # path to the final Info.plist file in the build cache
        set(FINAL_PLIST_PATH ${MAC_CACHE_PATH}/Info.plist)
        # find the icon-info.plist file actool will spit out
        set(ICON_PLIST_PATH ${MAC_CACHE_PATH}/icon-info.plist)
        # find the .icns file actool will spit out
        set(ICON_BUNDLE_PATH "${MAC_CACHE_PATH}/${target}.icns")

        # configure the Info.plist file and place it into the build cache
        configure_file("${CMAKE_SOURCE_DIR}/system/Info.plist.in"
                       "${GENERATED_PLIST_PATH}"
        )

        # make a command to run the asset compiler
        add_custom_command(
            OUTPUT "${ASSET_PACKAGE}" "${ICON_PLIST_PATH}" "${ICON_BUNDLE_PATH}"
            COMMAND xcrun actool
                "${ASSET_BUNDLE}"
                "${ICON_PATH}"
                --compile "${MAC_CACHE_PATH}"
                --output-format human-readable-text
                --notices
                --warnings
                --output-partial-info-plist "${ICON_PLIST_PATH}"
                --app-icon "${target}"
                --accent-color AccentColor
                --development-region en
                --target-device mac
                --minimum-deployment-target "${MINIMUM_MACOS_VERSION}"
                --platform macosx
            DEPENDS "${ASSET_BUNDLE}" "${ICON_PATH}"
            COMMENT "Compiling app icon and system assets for macOS"
        )

        add_custom_target(CompileMacAssets ALL
            DEPENDS "${ASSET_PACKAGE}"
            DEPENDS "${ICON_PLIST_PATH}"
            DEPENDS "${ICON_BUNDLE_PATH}"
        )
        add_dependencies(${target} CompileMacAssets)

        # set the compiled asset bundle as a target for the app and copy it into Resources
        target_sources(${target} PRIVATE "${ASSET_PACKAGE}")
        set_property(SOURCE "${ASSET_PACKAGE}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources")

        # add the icon bundle to the target and copy it into Resources
        target_sources(${target} PRIVATE "${ICON_BUNDLE_PATH}")
        set_property(SOURCE "${ICON_BUNDLE_PATH}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources")

        # merge the icon-info.plist into the main Info.plist
        add_custom_command(
            OUTPUT "${FINAL_PLIST_PATH}"
            COMMAND ${CMAKE_COMMAND} -E copy "${GENERATED_PLIST_PATH}" "${FINAL_PLIST_PATH}"
            COMMAND /usr/libexec/PlistBuddy -c "Merge '${ICON_PLIST_PATH}'" "${FINAL_PLIST_PATH}"
            DEPENDS "${ICON_PLIST_PATH}" "${GENERATED_PLIST_PATH}"
            COMMENT "Merging icon-info.plist into main Info.plist"
        )

        add_custom_target(InfoPlistMerge ALL
            DEPENDS "${FINAL_PLIST_PATH}"
        )
        add_dependencies(${target} InfoPlistMerge)

        # add the Info.plist to the target and have it depend on the icon
        set_property(
            SOURCE "${FINAL_PLIST_PATH}"
            PROPERTY MACOSX_PACKAGE_LOCATION "Contents/"
        )

        # tell Xcode this is the bundle Info.plist
        set_target_properties(${target} PROPERTIES
            XCODE_ATTRIBUTE_INFOPLIST_FILE ${FINAL_PLIST_PATH}
        )

        message(STATUS "Compiling icon for macOS ${MINIMUM_MACOS_VERSION}: ${ICON_PATH} -> Resources/${target}.icns")
        message(STATUS "Compiling assets for macOS ${MINIMUM_MACOS_VERSION}: ${ASSET_BUNDLE} -> Resources/Assets.car")
        message(STATUS "Added macOS icon property list")
    else()
        set(DESKTOP_FILE "${CMAKE_BINARY_DIR}/${target}.desktop")
        set(ICON_NAME "$ENV{HOME}/.local/share/pixmaps/${target}.png")
        set(ICON_SOURCE "${CMAKE_SOURCE_DIR}/system/assets/icon.png")
        set(ICON_TARGET "${CMAKE_BINARY_DIR}/${ICON_NAME}.png")
        set(EXEC_PATH "$ENV{HOME}/.local/bin/${target}")

        # Configure .desktop file from template
        configure_file("${CMAKE_SOURCE_DIR}/system/${target}.desktop.in"
                    "${DESKTOP_FILE}"
                    @ONLY)
        configure_file("${ICON_SOURCE}" "${ICON_TARGET}" COPYONLY)

        # Install the .desktop file and icon
        install(TARGETS ${target}
            RUNTIME DESTINATION bin
            COMPONENT Runtime
        )
        install(FILES "${DESKTOP_FILE}" DESTINATION share/applications)
        install(FILES "${ICON_TARGET}" DESTINATION share/pixmaps)
    endif()
endfunction()


function(SyngineGame name)
# Parse args
set(options "")
set(oneValueArgs "")
set(multiValueArgs SOURCES)
cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

if(NOT ARG_SOURCES)
    message(FATAL_ERROR "SyngineGame: No sources provided for target ${name}.")
endif()

# Create executable
add_executable(${name} "${ARG_SOURCES}")
target_link_libraries(${name} PRIVATE Syngine)

# Game's source dir is assumed to be it's CMakeLists.txt location
set(GAME_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Add platform specific flags (this seems to cause issues on occasion, may be removed in future)
if(APPLE)
    target_compile_definitions(${name} PRIVATE BX_PLATFORM_OSX=1)
elseif(UNIX AND NOT APPLE)
    # target_compile_definitions(${name} PRIVATE BX_PLATFORM_LINUX=1)
endif()

if(MSVC)
    target_compile_options(${name} PRIVATE
        /Zc:__cplusplus
        /Zc:preprocessor
    )
    set(BX_COMPILE_DEBUG 1)
endif()

target_compile_definitions(${name} PRIVATE
    "$<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>"       # Defines BX_CONFIG_DEBUG as 1 for Debug
    "$<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>" # Defines BX_CONFIG_DEBUG as 0 for others
)
message(STATUS "SyngineGame: Set BX_CONFIG_DEBUG preprocessor definition for ${name}.")

# Add default shaders
set(ALL_COMPILED_SHADER_BINARIES "")
compile_collect_shaders("${SYNGINE_SOURCE_DIR}/default/shaders" ALL_COMPILED_SHADER_BINARIES)

# Discover and process game asset folders automatically.
set(ALL_BUNDLED_MESH_FILES "")
set(ALL_BUNDLED_OTHER_ASSET_FILES "")
if(EXISTS "${GAME_ASSET_DIR}")
    file(GLOB GAME_ASSET_SUBDIRS RELATIVE "${GAME_ASSET_DIR}" "${GAME_ASSET_DIR}/*")
    foreach(ASSET_SUBDIR ${GAME_ASSET_SUBDIRS})
        if(NOT IS_DIRECTORY "${GAME_ASSET_DIR}/${ASSET_SUBDIR}")
            continue()
        endif()

        if(ASSET_SUBDIR STREQUAL "shaders")
            compile_collect_shaders("${GAME_ASSET_DIR}/${ASSET_SUBDIR}" ALL_COMPILED_SHADER_BINARIES)
        elseif(ASSET_SUBDIR STREQUAL "meshes")
            compile_all_meshes(
                SOURCE_DIRECTORY "${GAME_ASSET_DIR}/${ASSET_SUBDIR}"
                OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/rom/meshes"
                BUNDLE_FILES_OUTPUT_VAR ALL_BUNDLED_MESH_FILES
            )
        else()
            compile_collect_asset_bundle(
                "${GAME_ASSET_DIR}/${ASSET_SUBDIR}"
                "${ASSET_SUBDIR}"
                GENERATED_ASSET_BUNDLE
            )
            if(GENERATED_ASSET_BUNDLE)
                list(APPEND ALL_BUNDLED_OTHER_ASSET_FILES ${GENERATED_ASSET_BUNDLE})
            endif()
        endif()
    endforeach()
endif()

message(STATUS "SyngineGame: Compiled mesh bundles for ${name}: ${ALL_BUNDLED_MESH_FILES}")
message(STATUS "SyngineGame: Compiled asset bundles for ${name}: ${ALL_BUNDLED_OTHER_ASSET_FILES}")

set(ALL_BUNDLED_GIZMO_FILES "")
compile_collect_default_gizmos(ALL_BUNDLED_GIZMO_FILES)
message(STATUS "SyngineGame: Compiled default gizmo bundles for ${name}: ${ALL_BUNDLED_GIZMO_FILES}")

# Add dependency on the compiled shaders
if(ALL_COMPILED_SHADER_BINARIES)
    add_custom_target(GameShaders ALL DEPENDS ${ALL_COMPILED_SHADER_BINARIES})
    add_dependencies(${name} GameShaders)
    message(STATUS "SyngineGame: Added GameShaders target for ${name}.")
endif()

# Add dependency on the compiled mesh bundles
if(ALL_BUNDLED_MESH_FILES)
    add_custom_target(GameMeshes ALL DEPENDS ${ALL_BUNDLED_MESH_FILES})
    add_dependencies(${name} GameMeshes)
    message(STATUS "SyngineGame: Added GameMeshes target for ${name}.")
endif()

# Add dependency on the compiled generic asset bundles
if(ALL_BUNDLED_OTHER_ASSET_FILES)
    add_custom_target(GameAssets ALL DEPENDS ${ALL_BUNDLED_OTHER_ASSET_FILES})
    add_dependencies(${name} GameAssets)
    message(STATUS "SyngineGame: Added GameAssets target for ${name}.")
endif()

# Add dependency on the compiled gizmo bundles
if(ALL_BUNDLED_GIZMO_FILES)
    add_custom_target(GameGizmos ALL DEPENDS ${ALL_BUNDLED_GIZMO_FILES})
    add_dependencies(${name} GameGizmos)
    message(STATUS "SyngineGame: Added GameGizmos target for ${name}.")
endif()

if(ALL_COMPILED_SHADER_BINARIES OR ALL_BUNDLED_MESH_FILES OR ALL_BUNDLED_OTHER_ASSET_FILES OR ALL_BUNDLED_GIZMO_FILES)
    set_property(TARGET ${name} PROPERTY SYNGINE_COPY_ROM TRUE)
endif()

# --- Target Properties (macOS Bundle Info) ---
if(APPLE)
    set_target_properties(${name} PROPERTIES
        MACOSX_BUNDLE TRUE
        XCODE_GENERATE_SCHEME TRUE
        XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.sentytek.bakerman"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.sentytek.bakerman"
        XCODE_ATTRIBUTE_SDKROOT "macosx"
        # tells Xcode to recognize the app icon. this might not be needed idk
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "${name}"
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_INCLUDE_ALL_APPICON_ASSETS TRUE
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

# Post build asset copying for win/linux
add_assets(${name})

# Add version
include(${SYNGINE_SOURCE_DIR}/cmake/versioning.cmake)
define_version()

endfunction()
