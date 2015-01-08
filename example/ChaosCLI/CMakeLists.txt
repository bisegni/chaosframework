cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

SET(chaos_cli_src main.cpp)

ADD_EXECUTABLE(ChaosCLI ${chaos_cli_src})


IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(ChaosCLI chaos_uitoolkit chaos_common ${FrameworkLib})
INSTALL_TARGETS(/bin ChaosCLI)