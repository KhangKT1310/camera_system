include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(hal_interface STATIC
    src/hal_error.c
)

add_library(camera_system::hal_interface ALIAS hal_interface)

target_include_directories(hal_interface PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(hal_interface PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    EXPORT_NAME hal_interface
)

# Install rules for library and public headers
install(TARGETS hal_interface
    EXPORT CameraSystemHALTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/hal
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# CMake package configuration for downstream consumption
set(HAL_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/CameraSystemHAL")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CameraSystemHALConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CameraSystemHALConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/CameraSystemHALConfig.cmake"
    INSTALL_DESTINATION "${HAL_CONFIG_INSTALL_DIR}"
)

install(EXPORT CameraSystemHALTargets
    FILE CameraSystemHALTargets.cmake
    NAMESPACE camera_system::
    DESTINATION "${HAL_CONFIG_INSTALL_DIR}"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CameraSystemHALConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/CameraSystemHALConfigVersion.cmake"
    DESTINATION "${HAL_CONFIG_INSTALL_DIR}"
)
