cmake_minimum_required(VERSION 3.20)
project(camera_device_agent C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerWarnings)

option(ENABLE_MOCK "Enable mock platform build" ON)
option(ENABLE_TESTS "Enable test suite build" ON)
option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" ON)

# Subdirectories
add_subdirectory(core/hal)

if(TARGET hal_interface)
    set_project_compiler_warnings(hal_interface)
endif()

if(ENABLE_MOCK)
    add_subdirectory(platform/mock)
    if(TARGET hal_platform_mock)
        set_project_compiler_warnings(hal_platform_mock)
    endif()
endif()

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
