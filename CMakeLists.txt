cmake_minimum_required (VERSION 2.6)
project (chaos)

SET( ${PROJECT_NAME}_MAJOR_VERSION 0 ) 
SET( ${PROJECT_NAME}_MINOR_VERSION 1 ) 
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )

OPTION( BUILD_FORCE_32 "Force 32 bit compilation" OFF )
OPTION( BUILD_PREFIX "Build Prefix" usr/local/lib )

EXECUTE_PROCESS(COMMAND chaos/common/version.sh)

INCLUDE_DIRECTORIES(. ${BUILD_PREFIX}/include /usr/local/include /usr/include) 
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/usr/local/lib /usr/local/lib /usr/lib)
ADD_DEFINITIONS(-g -O2 -Wall)
    
IF( BUILD_FORCE_32 ) 
    MESSAGE( STATUS "Forcing 32 bit compilation" )
    set (CMAKE_C_FLAGS "-m32 -march=i686")
    set (CMAKE_CXX_FLAGS "-m32 -march=i686")
    set (CMAKE_LINK_FLAGS "-m32 -march=i686")
ENDIF()



find_library(BOOST_PO boost_program_options PATHS ${BUILD_PREFIX}/lib,/usr/lib,/usr/local/lib DOC "boost_program_options library")
if(NOT BOOST_PO)
    message(FATAL_ERROR "Library boost_program_options required, but not found!")
endif(NOT BOOST_PO)

find_library(BOOST_SY boost_system PATHS ${BUILD_PREFIX}/lib,/usr/lib,/usr/local/lib DOC "boost_system library")
if(NOT BOOST_SY)
    message(FATAL_ERROR "Library boost_system required, but not found!")
endif(NOT BOOST_SY)

find_library(BOOST_TH boost_thread PATHS ${BUILD_PREFIX}/lib,/usr/lib,/usr/local/lib DOC "boost_thread library")
if(NOT BOOST_TH)
    message(FATAL_ERROR "Library boost_thread required, but not found!")
endif(NOT BOOST_TH)

find_library(BOOST_CH boost_chrono PATHS ${BUILD_PREFIX}/lib,/usr/lib,/usr/local/lib DOC "boost_chrono library")
if(NOT BOOST_CH)
    message(FATAL_ERROR "Library boost_chrono required, but not found!")
endif(NOT BOOST_CH)

find_library(BOOST_RE boost_regex PATHS ${BUILD_PREFIX}/lib,/usr/lib,/usr/local/lib DOC "boost_regex library")
if(NOT BOOST_RE)
    message(FATAL_ERROR "Library boost_regex required, but not found!")
endif(NOT BOOST_RE)

find_library(BOOST_LOG boost_log PATHS ${BUILD_PREFIX}/lib,/usr/lib,/usr/local/lib DOC "boost_log library")
if(NOT BOOST_LOG)
    message(FATAL_ERROR "Library boost_log required, but not found!")
endif(NOT BOOST_LOG)

find_library(BOOST_LOG_SET boost_log_setup PATHS /usr/lib,/usr/local/lib,usr/local/lib DOC "boost_log_setup library")
if(NOT BOOST_LOG_SET)
    message(FATAL_ERROR "Library boost_log_setup required, but not found!")
endif(NOT BOOST_LOG_SET)

find_library(MEMCACHED_LIB memcached PATHS ${BUILD_PREFIX}/lib,/usr/lib,/usr/local/lib DOC "memcached library")
if(NOT MEMCACHED_LIB)
    message(FATAL_ERROR "Library memcached required, but not found!")
endif(NOT MEMCACHED_LIB)

find_library(MSGPACK_LIB msgpack PATHS ${BUILD_PREFIX}/lib,/usr/lib,/usr/local/lib  DOC "msgpack library")
if(NOT MSGPACK_LIB)
    message(FATAL_ERROR "Library msgpack required, but not found!")
endif(NOT MSGPACK_LIB)

find_library(MSGPACK_RPC_LIB msgpack-rpc PATHS /usr/lib,/usr/local/lib,usr/local/lib  DOC "msgpack-rpc library")
if(NOT MSGPACK_RPC_LIB)
    message(FATAL_ERROR "Library msgpack-rpc required, but not found!")
endif(NOT MSGPACK_RPC_LIB)

find_library(MPIO_LIB mpio PATHS ${BUILD_PREFIX}/lib,/usr/lib,/usr/local/lib  DOC "mpio library")
if(NOT MPIO_LIB)
    message(FATAL_ERROR "Library mpio required, but not found!")
endif(NOT MPIO_LIB)

find_library(ZMQ_LIB zmq PATHS ${BUILD_PREFIX}/lib,/usr/lib,/usr/local/lib  DOC "zmq library for additional option in rpc and event")
if(ZMQ_LIB)
    message(STATUS "ZMQ library is found so additional rpc and event driver will be compiled")
     SET(DEV_WITH_ZMQ on)
    SET(common_lib_src ${common_lib_src} rpc/zmq/ZMQClient.cpp rpc/zmq/ZMQServer.cpp)
    SET(common_lib_library ${common_lib_library} zmq)
endif(ZMQ_LIB)

message(STATUS "Configure Common Layer")
ADD_SUBDIRECTORY(chaos/common bin/bin_common)

message(STATUS "Configure CUToolkit Layer")
ADD_SUBDIRECTORY(chaos/cu_toolkit bin/bin_cutoolkit)

message(STATUS "Configure UIToolkit Layer")
ADD_SUBDIRECTORY(chaos/ui_toolkit bin/bin_uitoolkit)

message(STATUS "Configure Chaos Node Directory Service")
ADD_SUBDIRECTORY(ChaosNodeDirectory bin/bin_cnd)

message(STATUS "Configure Chaos HST Service")
ADD_SUBDIRECTORY(ChaosHSTService bin/bin_chst)

message(STATUS "Configure Control Unit Test example")
ADD_SUBDIRECTORY(example/ControlUnitTest bin/bin_example_cutest)

message(STATUS "Configure UIToolkit command line example")
ADD_SUBDIRECTORY(example/UIToolkitCMDLineExample bin/bin_example_uicmdline_test)

message(STATUS "Configure UIToolkit c-wrapper command line example")
ADD_SUBDIRECTORY(example/UIToolkitCMDLineCWrapperExample bin/bin_example_uicmdline_cwrapper_test)

message(STATUS "Configure BenchTest simulation excutable")
ADD_SUBDIRECTORY(example/BenchTest bin/bench_test)
