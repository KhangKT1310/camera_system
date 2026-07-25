add_library(camera_system_signaling
    signaling_client/src/signaling_client.c
)

target_include_directories(camera_system_signaling PUBLIC
    signaling_client/include
)

if(COMMAND set_project_compiler_warnings)
    set_project_compiler_warnings(camera_system_signaling)
endif()
