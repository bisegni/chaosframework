cmake_minimum_required(VERSION 2.6)
option(BUILD_FORCE_32 "Set to ON to enable 32 bit compilation" OFF)

IF( ($ENV{CHAOS32}) OR (BUILD_FORCE_32) )
    MESSAGE(STATUS "Enabling 32 bit Compilation")
    set (CMAKE_C_FLAGS "-m32")
    set (CMAKE_CXX_FLAGS "-m32")
    set (CMAKE_LINK_FLAGS "-m32")
ENDIF()
ADD_DEFINITIONS(-g -O2 -Wall)

SET(cutest_exe_src ControlUnitExample.cpp RTWorkerCU.cpp SCWorkerCU.cpp SinWaveCommand.cpp Sl7TcpDriver.cpp sl7lib/nodave.c sl7lib/openSocket.c sl7lib/setport.c)
SET(swalim_src SWAlim/main.cpp SWAlim/SWAlimCU.cpp)

INCLUDE_DIRECTORIES(. ${PROJECT_SOURCE_DIR}/../../usr/local/include /usr/local/include /usr/include ${PROJECT_SOURCE_DIR}/example/ChaosDeviceDriver/) 
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../usr/local/lib /usr/local/lib)

ADD_EXECUTABLE(ControlUnitTest ${cutest_exe_src})
ADD_EXECUTABLE(CUSWAlim ${swalim_src})

TARGET_LINK_LIBRARIES(ControlUnitTest chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio) 
TARGET_LINK_LIBRARIES(CUSWAlim chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio chaosDD) 
