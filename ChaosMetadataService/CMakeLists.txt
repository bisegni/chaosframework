cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
SET(cnd_src main.cpp ChaosMetadataService.cpp)

SET(cnd_src  ${cnd_src}	data/DataManagment.cpp)

SET(cnd_src  ${cnd_src}	api_services/ApiServer.cpp api_services/DeviceApi.cpp)


ADD_EXECUTABLE(ChaosMetadataService ${cnd_src})

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()
TARGET_LINK_LIBRARIES(ChaosMetadataService chaos_common ${FrameworkLib})
INSTALL_TARGETS(/bin ChaosMetadataService)