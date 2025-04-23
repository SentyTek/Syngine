cmake_minimum_required(VERSION 3.16)
project(bakerman)

# Set policy CMP0177 to NEW to normalize install() DESTINATION paths
cmake_policy(SET CMP0177 NEW)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" CACHE INTERNAL "")

if(MSVC)
    if( NOT CMAKE_GENERATOR STREQUAL "Ninja")
        add_definitions(/MP)
    endif()
endif()

#set exe name
set(EXECUTABLE_NAME ${PROJECT_NAME})

add_executable(${EXECUTABLE_NAME})

# add the source files
target_sources(${EXECUTABLE_NAME}
PRIVATE
    src/SyngineCore.cpp
    src/SyngineGraphics.cpp
    src/SynOSXMetalBridge.mm
    test/main.cpp
)

# to get an app icon on Apple, add it to the executable, then the image file in info.plist.in
if(APPLE)
    target_sources("${EXECUTABLE_NAME}" PRIVATE "test/icon.png")
endif()

target_compile_features(${EXECUTABLE_NAME} PRIVATE cxx_std_20)

# add platform specific flags
if(APPLE)
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE BX_PLATFORM_OSX=1)
elseif(WIN32)
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE BX_PLATFORM_WINDOWS=1)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(${EXECUTABLE_NAME} PRIVATE BX_PLATFORM_LINUX=1)
endif()

# set what gets built
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)

set(BGFX_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
if(APPLE) # cocoa is really picky and requires entire renderer to be single threaded
set(BGFX_CONFIG_MULTITHREADED OFF CACHE BOOL "" FORCE)
endif()

if(APPLE)
    set(SDL_VIDEO_VULKAN OFF CACHE BOOL "" FORCE)
    set(SDL_VULKAN OFF CACHE BOOL "" FORCE)

    set(BGFX_CONFIG_RENDERER_METAL ON CACHE BOOL "Enable Metal renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_OPENGL OFF CACHE BOOL "Disable OpenGL renderer" FORCE)
    set(BGFX_CONFIG_RENDERER_VULKAN OFF CACHE BOOL "Disable Vulkan renderer" FORCE)
endif()

# configure SDL and bgfx by calling their cmake scripts
# EXECLUDE_FROM_ALL is used so that its install targets and configs cont pollute upwards into the parent project
add_subdirectory(third_party/SDL EXCLUDE_FROM_ALL)
add_subdirectory(third_party/bgfx.cmake EXCLUDE_FROM_ALL)

# link the libs to the executable
target_link_libraries(${EXECUTABLE_NAME} PRIVATE #if using satellite libs, SDL must be linked LAST
    SDL3::SDL3
    bgfx
    bx
    bimg
)
target_include_directories(${EXECUTABLE_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    third_party/bgfx.cmake/bgfx/include
    third_party/bgfx.cmake/bx/include
    third_party/bgfx.cmake/bimg/include
)

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
        set_property(SOURCE "${FILE}" MACOSX_PACKAGE_LOCATION "Resources/${relpath}")
    endmacro()
    # at this point we can add the resources but we aint got any
endif()

# set some extra configs per platform
set(RESOURCE_FILES "src/icon.png")
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    # on macOS, we set the bundle identifier and version
    MACOSX_BUNDLE TRUE
    #in Xcode, create a scheme in the dropdowns for the app
    XCODE_GENERATE_SCHEME TRUE
    XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.sentytek.bakerman"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.sentytek.bakerman"
    XCODE_ATTRIBUTE_CURRENTYEAR "${CURRENTYEAR}"
    RESOURCE "${RESOURCE_FILES}"
)

#on macOS, we need to ensure the bundle is valid by calling fixup_bundle
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
   #tell Install about the target, otherwise fixup will not work
   install(TARGETS ${EXECUTABLE_NAME}
       BUNDLE DESTINATION ./install COMPONENT Runtime # Installs to ${CMAKE_INSTALL_PREFIX}/install
       RUNTIME DESTINATION Contents/MacOS # BUNDLE destination handles the .app structure
   )

   set(DEP_DIR "${CMAKE_BINARY_DIR};${CMAKE_BINARY_DIR}/Release;${CMAKE_BINARY_DIR}/Debug") # Directory to search for dependencies
   # Construct the path to the bundle *after* installation
   set(BUNDLE_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/install/${EXECUTABLE_NAME}.app")
    INSTALL(CODE
        "message(STATUS \"Fixing up bundle at: ${BUNDLE_INSTALL_PATH}\")
         include(BundleUtilities)
          fixup_bundle(\"${BUNDLE_INSTALL_PATH}\" \"\" \"${DEP_DIR}\")"
       COMPONENT Runtime # Ensure fixup runs when the Runtime component (the bundle) is installed
    )

   set(CPACK_GENERATOR "DragNDrop")
   include(CPack)
endif()