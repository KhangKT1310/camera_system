enable_testing()

# Helper function to add test and enforce strict warnings
function(add_agent_test target_name source_file)
    add_executable(${target_name} ${source_file})
    target_link_libraries(${target_name} PRIVATE hal_platform_mock)
    if(COMMAND set_project_compiler_warnings)
        set_project_compiler_warnings(${target_name})
    endif()
    add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

add_agent_test(HALCapabilityTest test_hal_capability.c)
add_agent_test(HALCaptureMockTest test_hal_capture_mock.c)
add_agent_test(HALErrorsTest test_hal_errors.c)
add_agent_test(SDKConsumerTest test_sdk_consumer.c)
