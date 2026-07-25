add_library(camera_system_signaling
    signaling_client/src/json_parser.c
    signaling_client/src/signaling_client.c
    signaling_client/src/ws_transport.c
)

target_include_directories(camera_system_signaling PUBLIC
    signaling_client/include
)

if(COMMAND set_project_compiler_warnings)
    set_project_compiler_warnings(camera_system_signaling)
endif()
