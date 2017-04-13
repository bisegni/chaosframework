
cmake_minimum_required(VERSION 2.6)
# IF (DEFINED ENV{CHAOS_BUNDLE} )
#   include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
# ENDIF()


SET(wp_src main.cpp		main.cpp
						ChaosWANProxy.cpp
						DefaultWANInterfaceHandler.cpp)

SET(wp_src ${wp_src}	wan_interface/AbstractWANInterface.cpp
						wan_interface/AbstractWANInterfaceResponse.cpp
						wan_interface/BaseWANInterfaceHandler.cpp)

SET(wp_src ${wp_src}	# wan_interface/http/HTTPWANInterface.cpp
						wan_interface/http/HTTPUIInterface.cpp
						wan_interface/http/HTTPWANInterfaceRequest.cpp
						wan_interface/http/HTTPWANInterfaceStringResponse.cpp
						wan_interface/http/mongoose.c)


SET(wp_src ${wp_src}	api/AbstractApi.cpp
						api/AbstractApiGroup.cpp
						api/PersistenceAccessor.cpp
						api/producer/ProducerGroup.cpp
						api/producer/ProducerRegisterDatasetApi.cpp
						api/producer/ProducerInsertDatasetApi.cpp
                        api/producer/ProducerRegisterJsonApi.cpp
                        api/producer/ProducerInsertJsonApi.cpp
						api/producer/ProducerGetLastDatasetApi.cpp
						api/consumer/ConsumerGroup.cpp
						api/consumer/ConsumerCUApi.cpp
						api/consumer/ConsumerMDSApi.cpp
						)

SET(wp_src ${wp_src}	persistence/AbstractPersistenceDriver.cpp
						persistence/DefaultPersistenceDriver.cpp)

ADD_EXECUTABLE(ChaosWANProxy ${wp_src})
ADD_DEFINITIONS(-DMONGOOSE_ENABLE_THREADS)
IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(ChaosWANProxy chaos_common chaos_metadata_service_client  chaos_driver_misc ${FrameworkLib})
INSTALL_TARGETS(/bin ChaosWANProxy)
