cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

SET(wp_src main.cpp		main.cpp
						ChaosWANProxy.cpp
						DefaultWANInterfaceHandler.cpp
						ChaosBridge.cpp)

SET(wp_src ${wp_src}	wan_interface/AbstractWANInterface.cpp
						wan_interface/AbstractWANInterfaceResponse.cpp
						wan_interface/AbstractWANInterfaceHandler.cpp)

SET(wp_src ${wp_src}	wan_interface/http/HTTPWANInterface.cpp
						wan_interface/http/HTTPWANInterfaceRequest.cpp
						wan_interface/http/HTTPWANInterfaceStringResponse.cpp)

SET(wp_src ${wp_src}	api/AbstractApi.cpp
						api/AbstractApiGroup.cpp)

SET(wp_src ${wp_src}	persistence/AbstractPersistenceDriver.cpp)

ADD_EXECUTABLE(ChaosWANProxy ${wp_src})

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(ChaosWANProxy chaos_common ${FrameworkLib})
INSTALL_TARGETS(/bin ChaosWANProxy)