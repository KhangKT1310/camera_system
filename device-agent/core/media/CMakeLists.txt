add_library(camera_system_media
    media_pipeline/src/media_frame.c
    media_pipeline/src/media_pipeline.c
    rtp_packetizer/src/rtp_packetizer.c
)

target_include_directories(camera_system_media PUBLIC
    media_pipeline/include
    rtp_packetizer/include
)

target_link_libraries(camera_system_media PRIVATE hal_interface)

if(COMMAND set_project_compiler_warnings)
    set_project_compiler_warnings(camera_system_media)
endif()
