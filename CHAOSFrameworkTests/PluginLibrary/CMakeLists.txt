cmake_minimum_required(VERSION 2.6)
project(PluginLibrary)

option(BUILD_FORCE_32 "Set to ON to enable 32 bit compilation" OFF)

IF( ($ENV{CHAOS32}) OR (BUILD_FORCE_32) )
MESSAGE(STATUS "Enabling 32 bit Compilation")
set (CMAKE_C_FLAGS "-m32")
set (CMAKE_CXX_FLAGS "-m32")
set (CMAKE_LINK_FLAGS "-m32")
ENDIF()

SET(plugin_src PluginLibrary.cpp )

INCLUDE_DIRECTORIES(. ${PROJECT_SOURCE_DIR}/usr/local/include /usr/local/include /usr/include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/usr/local/lib /usr/local/lib)

ADD_LIBRARY(PluginLibrary SHARED ${plugin_src})
set_target_properties(PluginLibrary PROPERTIES PREFIX "" SUFFIX ".chaos_extension")


TARGET_LINK_LIBRARIES(PluginLibrary chaos_cutoolkit chaos_common ${PluginTestLib})

INSTALL_TARGETS(/bin PluginLibrary)
