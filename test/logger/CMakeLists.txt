add_executable(test_logger test_logger.cpp)

set_target_properties(test_logger
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

target_link_libraries(test_logger PRIVATE cantina::logger GTest::GTest GTest::Main)

add_test(NAME test_logger
         COMMAND test_logger)
