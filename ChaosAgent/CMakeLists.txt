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

	      SET(agent_src  ${agent_src}
		external_command_pipe/AbstractCommandOutputStream.cpp
		external_command_pipe/AbstractExternalCommand.cpp
		external_command_pipe/CommandEcho.cpp
		external_command_pipe/ExternaCommandExecutor.cpp
		external_command_pipe/FileCommandOutputStream.cpp
		external_command_pipe/GetUSStatistic.cpp)
	      
SET(agent_src  ${agent_src} utility/PipeLineReader.cpp utility/ProcUtil.cpp)
SET(agent_src  ${agent_src}	worker/ProcessWorker.cpp
                            worker/DeployWorker.cpp
                            worker/LogWorker.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${agent_src})

IF(BUILD_FORCE_STATIC)
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_common ${FrameworkLib})
INSTALL_TARGETS(/bin ${PROJECT_NAME})
