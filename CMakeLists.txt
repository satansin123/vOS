cmake_minimum_required(VERSION 3.15)

# Use fixed project name
set(PROJECT_NAME "vOS")
project(${PROJECT_NAME} VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "=== Build Configuration ===")
message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
if(NOT SOURCES)
    message(FATAL_ERROR "No source files found in src/ directory")
endif()

add_executable(${PROJECT_NAME} ${SOURCES})


set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()
