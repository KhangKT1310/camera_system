add_library(camera_system_streaming
    webrtc_transport/src/webrtc_transport.c
    webrtc_transport/src/webrtc_transport_libdatachannel.c
    livestream_session/src/livestream_session_controller.c
)

target_include_directories(camera_system_streaming PUBLIC
    webrtc_transport/include
    livestream_session/include
)

target_link_libraries(camera_system_streaming PUBLIC camera_system_media)

if(COMMAND set_project_compiler_warnings)
    set_project_compiler_warnings(camera_system_streaming)
endif()
