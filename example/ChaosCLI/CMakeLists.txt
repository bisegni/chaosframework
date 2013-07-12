cmake_minimum_required(VERSION 2.6)
option(BUILD_FORCE_32 "Set to ON to enable 32 bit compilation" OFF)

IF( ($ENV{CHAOS32}) OR (BUILD_FORCE_32) )
    MESSAGE(STATUS "Enabling 32 bit Compilation")
    set (CMAKE_C_FLAGS "-m32")
    set (CMAKE_CXX_FLAGS "-m32")
    set (CMAKE_LINK_FLAGS "-m32")
ENDIF()
ADD_DEFINITIONS(-g -O2 -Wall)

SET(chaos_cli_src main.cpp)

INCLUDE_DIRECTORIES(. ${PROJECT_SOURCE_DIR}/../../usr/local/include /usr/local/include /usr/include ${PROJECT_SOURCE_DIR}/example/ChaosCLI/) 
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../usr/local/lib /usr/local/lib)

ADD_EXECUTABLE(ChaosCLI ${cutest_exe_src})

TARGET_LINK_LIBRARIES(ChaosCLI chaos_uitoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio) 
