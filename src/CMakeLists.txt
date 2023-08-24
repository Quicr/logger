add_library(logger ansi.cpp logger.cpp)
add_library(cantina::logger ALIAS logger)

set_target_properties(logger
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        PREFIX "libcantina_")
target_compile_options(logger PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wpedantic -Wextra -Wall>
     $<$<CXX_COMPILER_ID:MSVC>: >)

# Specify the internal and public include directories
target_include_directories(logger
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

# Install target and associated include files
if(logger_INSTALL)
    install(TARGETS logger EXPORT loggerTargets ARCHIVE
            FRAMEWORK DESTINATION ${CMAKE_INSTALL_DATADIR}/cantina)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ TYPE INCLUDE)
    install(EXPORT loggerTargets
            FILE loggerTargets.cmake
            NAMESPACE LOGGER::
            DESTINATION lib/cmake/logger/)
endif()
