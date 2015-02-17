cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

SET(chaos_cde_src cde.cpp)

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

ADD_EXECUTABLE(ChaosDataExport ${chaos_cde_src})


TARGET_LINK_LIBRARIES(ChaosDataExport chaos_uitoolkit chaos_common ${FrameworkLib})

INSTALL_TARGETS(/bin ChaosDataExport)