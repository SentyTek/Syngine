cmake_minimum_required(VERSION 3.10)
project(bakerman)
set(EXECUTABLE_NAME ${PROJECT_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
include(clangdConfig.cmake)
include(bgfxshaderc.cmake)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

add_subdirectory(engine)

if(MSVC)
    message(STATUS "Root: Forcing /MTd for bakerman's CMAKE_CXX_FLAGS_DEBUG just before target definition.")
    if(CMAKE_CXX_FLAGS_DEBUG MATCHES "/MDd")
        string(REPLACE "/MDd" "/MTd" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    elseif(NOT CMAKE_CXX_FLAGS_DEBUG MATCHES "/MTd")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    endif()
    # Ensure /MD is not in debug flags
    string(REPLACE "/MD" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    message(STATUS "Root: CMAKE_CXX_FLAGS_DEBUG for bakerman is now: ${CMAKE_CXX_FLAGS_DEBUG}")

    # You might also need to apply this specifically to the target if global flags are tricky
    # target_compile_options(${EXECUTABLE_NAME} PRIVATE "$<$<CONFIG:Debug>:/MTd>")
    # Be careful with the above, as target_compile_options appends.
    # A more robust way for a target is to set its property:
    # set_property(TARGET ${EXECUTABLE_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS_DEBUG " /MTd") # Not standard, check exact property
    # The standard way is to ensure CMAKE_CXX_FLAGS_DEBUG is correct before the target uses it.
endif()

add_executable(${EXECUTABLE_NAME}
    src/main.cpp
)

# Include the official bgfx shader utils
# include(${CMAKE_SOURCE_DIR}/third_party/bgfx.cmake/cmake/bgfxToolUtils.cmake)

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

# Include directories for the executable
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    # ${CMAKE_SOURCE_DIR}/engine/include # Should be inherited via PUBLIC from Syngine
    ${CMAKE_CURRENT_SOURCE_DIR}/src # If main.cpp includes headers from root src
    # Add third-party includes ONLY if main.cpp includes them DIRECTLY
    # ${CMAKE_SOURCE_DIR}/third_party/SDL/include # Example
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE Syngine)

# add platform specific flags
if(APPLE)
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE BX_PLATFORM_OSX=1)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE BX_PLATFORM_LINUX=1)
endif()

if(MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE
        /Zc:__cplusplus
        /Zc:preprocessor
    )
    set(BX_COMPILE_DEBUG 1)
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
        "$<$<CONFIG:Debug>:BX_CONFIG_DEBUG=1>"       # Defines BX_CONFIG_DEBUG as 1 for Debug
        "$<$<NOT:$<CONFIG:Debug>>:BX_CONFIG_DEBUG=0>" # Defines BX_CONFIG_DEBUG as 0 for others
    )
    message(STATUS "Root: Set BX_CONFIG_DEBUG preprocessor definition for ${EXECUTABLE_NAME}.")
endif()

clangdConfigure(${EXECUTABLE_NAME})

# to get an app icon on Apple, add it to the executable, then the image file in info.plist.in
if(APPLE)
    target_sources("${EXECUTABLE_NAME}" PRIVATE "src/icon.png")
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
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    XCODE_GENERATE_SCHEME TRUE
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.sentytek.bakerman"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.sentytek.bakerman"
    RESOURCE "${RESOURCE_FILES_FOR_TARGET}"
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
        # Common choices for Windows: dx11, dx12, vulkan (spirv)
        # Choose the one that matches your BGFX renderer for Windows
        set(SHADER_PLATFORM_SUBDIR_GENERIC "dx11") # Example
    else() # Linux
        # Common choices for Linux: glsl, spirv (vulkan)
        set(SHADER_PLATFORM_SUBDIR_GENERIC "spirv") # Example
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

    # Copy icon
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/icon.png")
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                "${CMAKE_CURRENT_SOURCE_DIR}/src/icon.png"
                "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/icon.png"
            COMMENT "Copying icon.png to executable directory"
        )
    endif()
endif()