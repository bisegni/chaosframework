cmake_minimum_required(VERSION 2.6)
project(lua)
IF (DEFINED ENV{CHAOS_BUNDLE} )
  include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
ELSE()
  MESSAGE(STATUS "STANDALONE, environment CHAOS_BUNDLE defined")
  IF (NOT CMAKE_INSTALL_PREFIX) 
    MESSAGE(fatal "you must add -DCMAKE_INSTALL_PREFIX=<dir>")
  ELSE()
    MESSAGE(STATUS "install prefix ${CMAKE_INSTALL_PREFIX}")
  ENDIF()
  INCLUDE_DIRECTORIES(. ../..)
  LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/build ${CMAKE_INSTALL_PREFIX}/lib)
ENDIF()

ADD_DEFINITIONS(-O2 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_LINUX -fPIC)
FILE(GLOB src "*.c")

ADD_LIBRARY(${PROJECT_NAME} STATIC ${src})

FILE(GLOB headers "core/*.h")
INSTALL(FILES ${headers} DESTINATION include/lua)
INSTALL_TARGETS(/lib  ${PROJECT_NAME})

