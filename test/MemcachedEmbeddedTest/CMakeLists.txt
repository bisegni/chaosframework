cmake_minimum_required(VERSION 2.6)
option(BUILD_FORCE_32 "Set to ON to enable 32 bit compilation" OFF)

IF (DEFINED ENV{CHAOS_BUNDLE} )
  include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
ELSE()
  SET(FrameworkLib boost_program_options boost_date_time boost_system  boost_chrono boost_regex boost_log_setup boost_log boost_filesystem boost_thread boost_atomic zmq.a jsoncpp pthread rt dl)
ENDIF()

IF( ($ENV{CHAOS32}) OR (BUILD_FORCE_32) )
    MESSAGE(STATUS "Enabling 32 bit Compilation")
    set (CMAKE_C_FLAGS "-m32")
    set (CMAKE_CXX_FLAGS "-m32")
    set (CMAKE_LINK_FLAGS "-m32")
ENDIF()
ADD_DEFINITIONS(-g -O2)

SET(memcache_test MemcachedEmbeddedTest/main.cpp)
SET(lockfree_test LockfreeCacheEmbeddedTest/main.cpp)

INCLUDE_DIRECTORIES(. ${PROJECT_SOURCE_DIR}/../../usr/local/include /usr/local/include /usr/include) 
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../usr/local/lib /usr/local/lib)

SET(ChachePerfomanceLib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(ChachePerfomanceLib)


ADD_EXECUTABLE(MemcachedEmbeddedTest ${memcache_test})
TARGET_LINK_LIBRARIES(MemcachedEmbeddedTest chaos_common boost_timer pthread dl ${ChachePerfomanceLib} )

ADD_EXECUTABLE(ChaosLockfreeCacheTest ${lockfree_test})
TARGET_LINK_LIBRARIES(ChaosLockfreeCacheTest chaos_common boost_timer pthread dl ${ChachePerfomanceLib} ) 
