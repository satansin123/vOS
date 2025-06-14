cmake_minimum_required(VERSION 3.15)
project(CppProject VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dynamically determine build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Dynamic path configuration based on environment
if(WIN32)
    # Windows specific paths
    set(ROOT_DIR ${CMAKE_SOURCE_DIR})
    set(BUILD_ROOT ${CMAKE_BINARY_DIR})
    set(SRC_ROOT ${CMAKE_SOURCE_DIR}/src)
elseif(UNIX AND NOT APPLE)
    # Linux/Container specific paths
    set(ROOT_DIR ${CMAKE_SOURCE_DIR})
    set(BUILD_ROOT ${CMAKE_BINARY_DIR})
    set(SRC_ROOT ${CMAKE_SOURCE_DIR}/src)
else()
    # Default paths
    set(ROOT_DIR ${CMAKE_SOURCE_DIR})
    set(BUILD_ROOT ${CMAKE_BINARY_DIR})
    set(SRC_ROOT ${CMAKE_SOURCE_DIR}/src)
endif()

# Print environment info for debugging
message(STATUS "=== Build Environment ===")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Root Directory: ${ROOT_DIR}")
message(STATUS "Build Directory: ${BUILD_ROOT}")
message(STATUS "Source Directory: ${SRC_ROOT}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# Create executable
add_executable(${PROJECT_NAME})

# Add source files dynamically
file(GLOB_RECURSE SOURCES "${SRC_ROOT}/*.cpp" "${SRC_ROOT}/*.c")
if(SOURCES)
    target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
    message(STATUS "Found sources: ${SOURCES}")
else()
    # Fallback to specific file
    target_sources(${PROJECT_NAME} PRIVATE ${SRC_ROOT}/main.cpp)
endif()

# Dynamic output directory configuration
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BUILD_ROOT}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${BUILD_ROOT}/lib
    LIBRARY_OUTPUT_DIRECTORY ${BUILD_ROOT}/lib
)

# Environment-specific compiler settings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4996)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Dynamic installation paths
if(WIN32)
    set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}/install")
else()
    set(CMAKE_INSTALL_PREFIX "${ROOT_DIR}/install")
endif()

# Install configuration
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
