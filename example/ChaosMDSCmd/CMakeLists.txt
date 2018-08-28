cmake_minimum_required(VERSION 2.6)
# IF (DEFINED ENV{CHAOS_BUNDLE} )
#   include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
# ENDIF()

SET(chaos_src main.cpp)

ADD_EXECUTABLE(ChaosMDSCmd ${chaos_src})


IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(ChaosMDSCmd chaos_uitoolkit chaos_common chaos_metadata_service_client ${FrameworkLib})
INSTALL_TARGETS(/bin ChaosMDSCmd)
