cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
SET(cnd_src main.cpp ChaosMetadataService.cpp)

ADD_EXECUTABLE(ChaosNodeDirectory ${cnd_src})

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
  TARGET_LINK_LIBRARIES(ChaosNodeDirectory ${FrameworkLib})
ELSE()
  TARGET_LINK_LIBRARIES(ChaosNodeDirectory chaos_common ${FrameworkLib})
ENDIF()

INSTALL_TARGETS(/bin ChaosNodeDirectory)