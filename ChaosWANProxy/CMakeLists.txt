cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
SET(wp_src main.cpp ChaosWANProxy.cpp)

ADD_EXECUTABLE(ChaosWANProxy ${wp_src})

IF(BUILD_FORCE_STATIC)
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
TARGET_LINK_LIBRARIES(ChaosWANProxy ${FrameworkLib})
ELSE()
TARGET_LINK_LIBRARIES(ChaosWANProxy chaos_common ${FrameworkLib})
ENDIF()

INSTALL_TARGETS(/bin ChaosWANProxy)