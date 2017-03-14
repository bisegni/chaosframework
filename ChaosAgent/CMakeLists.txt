cmake_minimum_required(VERSION 2.8)
project(ChaosAgent)
IF (DEFINED ENV{CHAOS_BUNDLE} )
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
ENDIF()

INCLUDE_DIRECTORIES(chaos/common ../)

SET(agent_src   main.cpp
                ChaosAgent.cpp
                AgentRegister.cpp
                AbstractWorker.cpp)

ADD_DEFINITIONS(-std=c++11)

SET(agent_src  ${agent_src}	worker/ProcessWorker.cpp
                            worker/DeployWorker.cpp
                            worker/LogWorker.cpp
                            worker/ProcUtil.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${agent_src})

IF(BUILD_FORCE_STATIC)
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_common ${FrameworkLib})
INSTALL_TARGETS(/bin ${PROJECT_NAME})
