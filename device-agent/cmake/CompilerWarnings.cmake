function(set_project_compiler_warnings target_name)
    option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" ON)

    set(COMMON_WARNINGS
        -Wall
        -Wextra
        -Wshadow
        -Wcast-align
        -Wunused
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
    )

    set(C_WARNINGS
        ${COMMON_WARNINGS}
        -Wimplicit-function-declaration
    )

    set(CXX_WARNINGS
        ${COMMON_WARNINGS}
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
    )

    if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES ".*Clang")
        list(APPEND C_WARNINGS -Wmisleading-indentation)
    endif()

    if(ENABLE_WARNINGS_AS_ERRORS)
        set(C_WARNINGS ${C_WARNINGS} -Werror)
        set(CXX_WARNINGS ${CXX_WARNINGS} -Werror)
    endif()

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4 $<$<BOOL:${ENABLE_WARNINGS_AS_ERRORS}>:/WX>)
    else()
        target_compile_options(${target_name} PRIVATE
            $<$<COMPILE_LANGUAGE:C>:${C_WARNINGS}>
            $<$<COMPILE_LANGUAGE:CXX>:${CXX_WARNINGS}>
        )
    endif()
endfunction()
