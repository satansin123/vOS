cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME "vOS")
project(${PROJECT_NAME} VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

message(STATUS "=== Build Configuration ===")
message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
if(NOT SOURCES)
    message(FATAL_ERROR "No source files found in src/ directory")
endif()

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src drivers)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE kernel32)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# Build drivers as DLLs in the correct location
file(GLOB DRIVER_SOURCES "drivers/*.cpp")
foreach(driver_file ${DRIVER_SOURCES})
    get_filename_component(driver_name ${driver_file} NAME_WE)
    add_library(${driver_name} SHARED ${driver_file})
    
    target_include_directories(${driver_name} PRIVATE drivers)
    target_link_libraries(${driver_name} PRIVATE Threads::Threads)
    
    if(WIN32)
        target_link_libraries(${driver_name} PRIVATE kernel32)
    endif()
    
    # Put DLLs in bin/drivers/ directory (where app expects them)
    set_target_properties(${driver_name} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/drivers
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/drivers
    )
endforeach()

# Create the drivers directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/drivers)
