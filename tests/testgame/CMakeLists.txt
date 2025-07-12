cmake_minimum_required(VERSION 3.10)
project(bakerman)
set(EXECUTABLE_NAME ${PROJECT_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(engine)

SyngineGame(${EXECUTABLE_NAME}
    src/main.cpp
)

# Keep own version of clangdConfig, for... idk, reasons?
include("${SYNGINE_SOURCE_DIR}/cmake/clangdConfig.cmake")
clangdConfigure(${EXECUTABLE_NAME})