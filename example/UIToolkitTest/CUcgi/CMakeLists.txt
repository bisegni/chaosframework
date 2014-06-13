cmake_minimum_required(VERSION 2.6)
option(BUILD_FORCE_32 "Set to ON to enable 32 bit compilation" OFF)


IF( ($ENV{CHAOS32}) OR (BUILD_FORCE_32) )
    MESSAGE(STATUS "Enabling 32 bit Compilation")
    set (CMAKE_C_FLAGS "-m32")
    set (CMAKE_CXX_FLAGS "-m32")
    set (CMAKE_LINK_FLAGS "-m32")
ENDIF()

if(NOT DEFINED ENV{CHAOS_FRAMEWORK} )
set(CHAOS_FRAMEWORK ../../../)
MESSAGE(WARNING "environment CHAOS_FRAMEWORK not set using ${CHAOS_FRAMEWORK}")
ELSE()
set(CHAOS_FRAMEWORK ../../../ $ENV{CHAOS_FRAMEWORK})
MESSAGE(STATUS "using framework in ${CHAOS_FRAMEWORK}")
ENDIF()


set(CMAKE_INSTALL_PREFIX $(CHAOS_FRAMEWORK)/usr/local)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

ADD_DEFINITIONS(-g -O2 -Wall)
add_definitions(-DBOOST_ALL_DYN_LINK -DCHAOS -DDEBUG)

SET(CGI main.cpp)


INCLUDE_DIRECTORIES(../../ ${CHAOS_FRAMEWORK}/usr/local/include)
LINK_DIRECTORIES(${CHAOS_FRAMEWORK}/usr/local/lib)

ADD_EXECUTABLE(cu.cgi ${CGI})


TARGET_LINK_LIBRARIES(cu.cgi cgicc pthread common_test chaos_uitoolkit driver_powersupply chaos_common common_debug boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached)

INSTALL_TARGETS(/bin cu.cgi)



