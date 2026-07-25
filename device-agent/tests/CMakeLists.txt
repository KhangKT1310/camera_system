enable_testing()

# Helper function to add test and enforce strict warnings
function(add_agent_test target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE 
        hal_platform_mock 
        camera_system_media 
        camera_system_streaming 
        camera_system_signaling
    )
    if(COMMAND set_project_compiler_warnings)
        set_project_compiler_warnings(${target_name})
    endif()
    add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

add_agent_test(HALCapabilityTest test_hal_capability.c)
add_agent_test(HALCaptureMockTest test_hal_capture_mock.c)
add_agent_test(HALErrorsTest test_hal_errors.c)
add_agent_test(SDKConsumerTest test_sdk_consumer.c)

# Phase 2 WebRTC Livestream MVP Tests
add_agent_test(RTPPacketizerTest test_rtp_packetizer.c)
add_agent_test(WebRTCTransportTest test_webrtc_transport.c)
add_agent_test(MalformedInputTest test_malformed_input.c)
add_agent_test(LivestreamShortTest test_livestream_short.c)

# Separate 24h Soak Test Executable (not registered under fast CTest)
add_executable(Livestream24hTest soak/test_livestream_24h.c)
target_link_libraries(Livestream24hTest PRIVATE 
    hal_platform_mock 
    camera_system_media 
    camera_system_streaming 
    camera_system_signaling
)
if(COMMAND set_project_compiler_warnings)
    set_project_compiler_warnings(Livestream24hTest)
endif()
