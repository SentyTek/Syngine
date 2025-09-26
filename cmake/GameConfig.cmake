# ╒═══════════════════ GameConfig.cmake ═╕
# │ Syngine                              │
# │ Created 2025-08-05                   │
# ├──────────────────────────────────────┤
# │ Copyright (c) SentyTek 2025-2025     │
# │ Placeholder License                  │
# ╰──────────────────────────────────────╯
# Adds several functions to compile shaders, add assets, and configure the game target (root/engine/cmake/GameConfig.cmake)

include(${SYNGINE_SOURCE_DIR}/cmake/CompileShaders.cmake)

# Function to compile all shaders in a directory
function(compile_collect_shaders SHADER_SRC_DIR ALL_SHADERS_LIST)
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
        set(${ALL_SHADERS_LIST} "${${ALL_SHADERS_LIST}};${COMPILED_SHADER_BINARIES}" PARENT_SCOPE)
    endif()
endfunction()

# Asset handling helpers
function(_add_assets_win_linux target)
    if(WIN32)
        set(SHADER_PLATFORM_SUBDIR "dx11")
    else()
        set(SHADER_PLATFORM_SUBDIR "spirv")
    endif()

    set(PLATFORM_SHADER_SUBDIR "${CMAKE_BINARY_DIR}/shaders/${SHADER_PLATFORM_SUBDIR}")
    if(EXISTS "${PLATFORM_SHADER_SUBDIR}")
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${PLATFORM_SHADER_SUBDIR}"
                "$<TARGET_FILE_DIR:${target}>/shaders"
            COMMENT "Copying ${SHADER_PLATFORM_SUBDIR} shaders to executable directory"
        )
    endif()

    # Loop through the game asset directory (root/assets) and copy all folders into the executable directory
    if(EXISTS "${GAME_ASSET_DIR}")
        file(GLOB GAME_ASSET_SUBDIRS RELATIVE "${GAME_ASSET_DIR}" "${GAME_ASSET_DIR}/*")
        foreach(SUBDIR ${GAME_ASSET_SUBDIRS})
            if(IS_DIRECTORY "${GAME_ASSET_DIR}/${SUBDIR}")
                # Do not copy the "shaders" directory
                if(SUBDIR STREQUAL "shaders")
                    continue()
                endif()
                add_custom_command(TARGET ${target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${GAME_ASSET_DIR}/${SUBDIR}"
                        "$<TARGET_FILE_DIR:${target}>/${SUBDIR}"
                    COMMENT "Copying game asset subdirectory '${SUBDIR}' to executable directory"
                )
                message(STATUS "Adding game asset subdirectory: ${GAME_ASSET_DIR}/${SUBDIR} -> $<TARGET_FILE_DIR:${target}>/${SUBDIR}")
            endif()
        endforeach()
    endif()

    # Loop through the engine default directory (root/engine/default) and copy all folders except "shaders" into the executable directory
    if(EXISTS "${SYNGINE_SOURCE_DIR}/default")
        file(GLOB ENGINE_DEFAULT_SUBDIRS RELATIVE "${SYNGINE_SOURCE_DIR}/default" "${SYNGINE_SOURCE_DIR}/default/*")
        foreach(SUBDIR ${ENGINE_DEFAULT_SUBDIRS})
            if(IS_DIRECTORY "${SYNGINE_SOURCE_DIR}/default/${SUBDIR}" AND NOT SUBDIR STREQUAL "shaders")
                add_custom_command(TARGET ${target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${SYNGINE_SOURCE_DIR}/default/${SUBDIR}"
                        "$<TARGET_FILE_DIR:${target}>/default/${SUBDIR}"
                    COMMENT "Copying engine default subdirectory '${SUBDIR}' to executable directory"
                )
                message(STATUS "Adding engine default subdirectory: ${SYNGINE_SOURCE_DIR}/default/${SUBDIR} -> $<TARGET_FILE_DIR:${target}>/default/${SUBDIR}")
            endif()
        endforeach()
    endif()
endfunction()

function(_add_assets_mac target)
    # macOS: Bundle resources into .app/Contents/Resources
    # Helper macro to add a file to the bundle's Resources directory
    macro(add_resource FILE DEST_SUBDIR)
        get_filename_component(FILENAME "${FILE}" NAME)
        target_sources(${target} PRIVATE "${FILE}")
        if(DEST_SUBDIR)
            set_property(SOURCE "${FILE}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${DEST_SUBDIR}")
        else()
            make_directory("${CMAKE_BINARY_DIR}/Resources/${DEST_SUBDIR}")
            set_property(SOURCE "${FILE}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${DEST_SUBDIR}")
        endif()
    endmacro()

    # Add shaders (metal)
    set(SHADER_PLATFORM_DIR "${CMAKE_BINARY_DIR}/shaders/metal")
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

    # --- Loop through the root/assets directory and copy all asset folders that aren't shaders ---
    if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
        file(GLOB ASSET_SUBDIRS RELATIVE "${CMAKE_SOURCE_DIR}/assets" "${CMAKE_SOURCE_DIR}/assets/*")
        foreach(ASSET_SUBDIR ${ASSET_SUBDIRS})
            if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/assets/${ASSET_SUBDIR}" AND NOT "${ASSET_SUBDIR}" STREQUAL "shaders")
                file(GLOB_RECURSE ASSET_SUBDIR_FILES RELATIVE "${CMAKE_SOURCE_DIR}/assets/${ASSET_SUBDIR}" "${CMAKE_SOURCE_DIR}/assets/${ASSET_SUBDIR}/*")
                foreach(asset_file_relative ${ASSET_SUBDIR_FILES})
                    set(abs_asset_file "${CMAKE_SOURCE_DIR}/assets/${ASSET_SUBDIR}/${asset_file_relative}")
                    if(IS_DIRECTORY "${abs_asset_file}")
                        continue()
                    endif()
                    target_sources(${target} PRIVATE "${abs_asset_file}")
                    set_property(SOURCE "${abs_asset_file}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${ASSET_SUBDIR}")
                    message(STATUS "Bundling asset file for macOS: ${abs_asset_file} -> Resources/${ASSET_SUBDIR}/${asset_file_relative}")
                endforeach()
            endif()
        endforeach()
    else()
        message(WARNING "Asset directory not found at ${CMAKE_SOURCE_DIR}/assets")
    endif()

    # --- Copy engine/default subfolders except 'shaders' into bundle Resources ---
    if(EXISTS "${SYNGINE_SOURCE_DIR}")
        file(GLOB ENGINE_DEFAULT_SUBDIRS RELATIVE "${SYNGINE_SOURCE_DIR}/default" "${SYNGINE_SOURCE_DIR}/default/*")
        foreach(subdir ${ENGINE_DEFAULT_SUBDIRS})
            if(IS_DIRECTORY "${SYNGINE_SOURCE_DIR}/default/${subdir}" AND NOT "${subdir}" STREQUAL "shaders")
                file(GLOB_RECURSE DEFAULT_SUBDIR_FILES RELATIVE "${SYNGINE_SOURCE_DIR}/default/${subdir}" "${SYNGINE_SOURCE_DIR}/default/${subdir}/*")
                foreach(default_file_relative ${DEFAULT_SUBDIR_FILES})
                    set(abs_default_file "${SYNGINE_SOURCE_DIR}/default/${subdir}/${default_file_relative}")
                    if(IS_DIRECTORY "${abs_default_file}")
                        continue()
                    endif()
                    target_sources(${target} PRIVATE "${abs_default_file}")
                    set_property(SOURCE "${abs_default_file}" PROPERTY MACOSX_PACKAGE_LOCATION "Resources/default/${subdir}")
                    message(STATUS "Bundling engine default file for macOS: ${abs_default_file} -> Resources/default/${subdir}/${default_file_relative}")
                endforeach()
            endif()
        endforeach()
    else()
        message(WARNING "Engine default directory not found at ${SYNGINE_SOURCE_DIR}/default")
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
set(multiValueArgs SOURCES SHADER_DIRS)
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

# Compile game shaders if provided
if(ARG_SHADER_DIRS)
    foreach(SHADER_DIR ${ARG_SHADER_DIRS})
        compile_collect_shaders("${SHADER_DIR}" ALL_COMPILED_SHADER_BINARIES)
    endforeach()
endif()

# Add dependency on the compiled shaders
if(ALL_COMPILED_SHADER_BINARIES)
    add_custom_target(GameShaders ALL DEPENDS ${ALL_COMPILED_SHADER_BINARIES})
    add_dependencies(${name} GameShaders)
    message(STATUS "SyngineGame: Added GameShaders target for ${name}.")
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
