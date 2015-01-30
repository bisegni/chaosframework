cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
# SET(cnd_src main.cpp ChaosMetadataService.cpp)

SET(cnd_src				main.cpp
						ChaosMetadataService.cpp)

SET(cnd_src  ${cnd_src}	api/ApiManagment.cpp
                        api/AbstractApi.cpp
                        api/AbstractApiGroup.cpp)

SET(cnd_src  ${cnd_src}	api/producer/ProducerApiGroup.cpp
						api/producer/ProducerRegisterApi.cpp)

SET(cnd_src  ${cnd_src}	api/unit_server/UnitServerApiGroup.cpp
                        api/unit_server/UnitServerRegisterApi.cpp)

SET(cnd_src  ${cnd_src}	persistence/AbstractPersistenceDriver.cpp
						persistence/AbstractDataAccess.cpp)

SET(cnd_src  ${cnd_src}	persistence/data_access/ProducerDataAccess.cpp
                        persistence/data_access/UnitServerDataAccess.cpp)

SET(cnd_src  ${cnd_src}	persistence/mongodb/MongoDBHAConnectionManager.cpp
						persistence/mongodb/MongoDBPersistenceDriver.cpp
						persistence/mongodb/MongoDBProducerDataAccess.cpp
					persistence/mongodb/MongoDBAccessor.cpp
                        persistence/mongodb/MongoDBUnitServerDataAccess.cpp)

SET(cnd_src  ${cnd_src}	batch/MDSBatchExecutor.cpp
                        batch/MDSBatchCommand.cpp)

ADD_EXECUTABLE(ChaosMetadataService ${cnd_src})

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()
TARGET_LINK_LIBRARIES(ChaosMetadataService chaos_common mongoclient ${FrameworkLib})
INSTALL_TARGETS(/bin ChaosMetadataService)
