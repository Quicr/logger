cmake_minimum_required(VERSION 3.17)

# Building tests by default depends on whether this is a subproject
if(DEFINED PROJECT_NAME)
    option(logger_BUILD_TESTS "Build Tests for the Logger Library" OFF)
else()
    option(logger_BUILD_TESTS "Build Tests for the Logger Library" ON)
endif()

# Option to control library installation
option(logger_INSTALL "Install the Logger Library" ON)

# Option to enable syslog support (on by default, except for WIN32)
if(NOT WIN32)
    option(logger_ENABLE_SYSLOG "Enable Logger's Syslog Support" ON)
else()
    option(logger_ENABLE_SYSLOG "Enable Logger's Syslog Support" OFF)
endif()

project(logger
        VERSION 1.1.0.0
        DESCRIPTION "Logger Library for C++ Projects"
        LANGUAGES CXX)

add_subdirectory(dependencies)
add_subdirectory(src)

include(CTest)

if(BUILD_TESTING AND logger_BUILD_TESTS)
    add_subdirectory(test)
endif()
