cmake_minimum_required(VERSION 2.6)
IF (DEFINED ENV{CHAOS_BUNDLE} )
  include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
ENDIF()

SET(mclt main.cpp NodeMonitor.cpp HandlerMonitor.cpp NodeSearchTest.cpp NodeMonitorHandlerTest.cpp)

ADD_EXECUTABLE(ChaosMetadataServiceClientTest ${mclt})

IF(BUILD_FORCE_STATIC)
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()
TARGET_LINK_LIBRARIES(ChaosMetadataServiceClientTest chaos_common chaos_metadata_service_client ${FrameworkLib})
INSTALL_TARGETS(/bin ChaosMetadataServiceClientTest)
