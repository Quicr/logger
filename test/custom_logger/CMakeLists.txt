add_executable(test_custom_logger test_custom_logger.cpp)

set_target_properties(test_custom_logger
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

# Is syslog enabled?
if(logger_ENABLE_SYSLOG)
    target_compile_definitions(test_custom_logger PRIVATE LOGGER_SYSLOG_ENABLED=1)
endif()

target_link_libraries(test_custom_logger PRIVATE cantina::logger GTest::GTest GTest::Main)

add_test(NAME test_custom_logger
         COMMAND test_custom_logger)
