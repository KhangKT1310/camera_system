add_library(hal_platform_mock STATIC
    src/mock_platform.c
)

add_library(camera_system::hal_platform_mock ALIAS hal_platform_mock)

target_link_libraries(hal_platform_mock PUBLIC hal_interface)

set_target_properties(hal_platform_mock PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    EXPORT_NAME hal_platform_mock
)

install(TARGETS hal_platform_mock
    EXPORT CameraSystemHALTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
