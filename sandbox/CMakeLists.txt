add_executable(openperf_sandbox
    main.cpp
)

target_link_libraries(openperf_sandbox
    PRIVATE openperf_core
)