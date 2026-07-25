cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Configure Core & Platform Subdirectories
add_subdirectory(core/hal)
add_subdirectory(core/media)
add_subdirectory(core/streaming)
add_subdirectory(core/signaling)
add_subdirectory(platform/mock)

# Standalone Camera Agent Executable
add_executable(camera_agent
    src/camera_agent.c
    src/main.c
)

target_include_directories(camera_agent PUBLIC
    src
    core/media/media_pipeline/include
    core/media/rtp_packetizer/include
    core/streaming/webrtc_transport/include
    core/streaming/livestream_session/include
    core/signaling/signaling_client/include
    core/hal/include
    core/hal/include/hal
)

target_link_libraries(camera_agent PRIVATE
    camera_system_media
    camera_system_streaming
    camera_system_signaling
    hal_interface
    hal_platform_mock
    pthread
)

if(COMMAND set_project_compiler_warnings)
    set_project_compiler_warnings(camera_agent)
endif()

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
