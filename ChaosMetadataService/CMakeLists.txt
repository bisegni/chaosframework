cmake_minimum_required(VERSION 2.8)
project(ChaosMetadataService)

option(COMPILE_HAZELCAST_LIVE_DRIVER "Compile hazelcast live data driver" OFF)
#option(COMPILE_NEW_MONGODB_DRIVER "Compile newongodb driver" OFF)

IF (DEFINED ENV{CHAOS_BUNDLE} )
  include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
ENDIF()


IF (COMPILE_HAZELCAST_LIVE_DRIVER)
  MESG("Install hazelcast c++ client")
  ExternalProject_Add(
     hazelcast
     GIT_REPOSITORY https://github.com/hazelcast/hazelcast-cpp-client.git
     GIT_TAG v3.9.1
     PREFIX "${CMAKE_BINARY_DIR}/ext_dep/hazelcast-prefix"
     SOURCE_DIR "${CMAKE_BINARY_DIR}/ext_dep/hazelcast-src"
     BINARY_DIR "${CMAKE_BINARY_DIR}/ext_dep/hazelcast-build"
     CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
      -DHZ_LIB_TYPE=STATIC
      -DHZ_BIT=64
      -DCMAKE_BUILD_TYPE=Release
     LOG_DOWNLOAD ON
     LOG_CONFIGURE ON
     LOG_BUILD ON)


     # Specify include dir
     ExternalProject_Get_Property(hazelcast source_dir)
     set(HAZELCAST_INCLUDE_DIR ${source_dir}/include)

     # Library
     ExternalProject_Get_Property(hazelcast binary_dir)
     set(HAZELCAST_LIBRARY_DIR ${binary_dir})

     MESG("${HAZELCAST_INCLUDE_DIR}")
     MESG("${HAZELCAST_LIBRARY_DIR}")
     MESG("${HAZELCAST_LIBRARY}")
ENDIF()


# IF(COMPILE_NEW_MONGODB_DRIVER)
MESG("Install c mongodb driver")
ExternalProject_Add(
   mongo-c-driver
   GIT_REPOSITORY https://github.com/mongodb/mongo-c-driver.git
   GIT_TAG 1.10.2
   PREFIX "${CMAKE_BINARY_DIR}/ext_dep/mongo-c-driver-prefix"
   SOURCE_DIR "${CMAKE_BINARY_DIR}/ext_dep/mongo-c-driver-src"
   BINARY_DIR "${CMAKE_BINARY_DIR}/ext_dep/mongo-c-driver-build"
   CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=Release
   LOG_DOWNLOAD ON
   LOG_CONFIGURE ON
   LOG_BUILD ON)

MESG("Install cxx mongodb driver")
ExternalProject_Add(
   mongo-cxx-driver
   DEPENDS mongo-c-driver
   GIT_REPOSITORY https://github.com/mongodb/mongo-cxx-driver
   GIT_TAG r3.3.0
   PREFIX "${CMAKE_BINARY_DIR}/ext_dep/mongo-cxx-driver-prefix"
   SOURCE_DIR "${CMAKE_BINARY_DIR}/ext_dep/mongo-cxx-driver-src"
   BINARY_DIR "${CMAKE_BINARY_DIR}/ext_dep/mongo-cxx-driver-build"
   CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=Release
   LOG_DOWNLOAD ON
   LOG_CONFIGURE ON
   LOG_BUILD ON)

   include_directories("${CMAKE_INSTALL_PREFIX}/include/mongocxx/v_noabi")
   include_directories("${CMAKE_INSTALL_PREFIX}/include/bsoncxx/v_noabi")
   include_directories("${CMAKE_INSTALL_PREFIX}/lib")
# ENDIF()

INCLUDE_DIRECTORIES(chaos/common ../)

SET(cnd_src				main.cpp
						      ChaosMetadataService.cpp
                  DriverPoolManager.cpp
                  QueryDataConsumer.cpp)

SET(cnd_src  ${cnd_src} ../chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.cpp
                        ../chaos_service_common/persistence/mongodb/MongoDBAccessor.cpp
                        ../chaos_service_common/persistence/mongodb/mongodb_cxx/MongoDBCXXDriver.cpp)

SET(cnd_src  ${cnd_src} ../chaos_service_common/persistence/data_access/AbstractDataAccess.cpp
						../chaos_service_common/persistence/data_access/AbstractPersistenceDriver.cpp)

SET(cnd_src  ${cnd_src}	cron_job/MDSCronusManager.cpp
                        cron_job/MDSCronJob.cpp
                        cron_job/management/MDSHistoryAgeingManagement.cpp)

SET(cnd_src  ${cnd_src}	common/CUCommonUtility.cpp
                        common/LogUtility.cpp)

SET(cnd_src  ${cnd_src}	api/ApiManagment.cpp
                        api/AbstractApi.cpp
                        api/AbstractApiGroup.cpp)

SET(cnd_src  ${cnd_src}	api/logging/LoggingApiGroup.cpp
                        api/logging/SubmitEntry.cpp
                        api/logging/SearchLogEntry.cpp
                        api/logging/GetLogForSourceUID.cpp
                        api/logging/GetLogDomainForSourceUID.cpp)

SET(cnd_src  ${cnd_src}	api/groups/GroupsApiGroup.cpp
                        api/groups/AddNode.cpp
                        api/groups/DeleteNode.cpp
                        api/groups/GetDomains.cpp
                        api/groups/GetNodeChilds.cpp)

SET(cnd_src  ${cnd_src}	api/service/ServiceApiGroup.cpp
                        api/service/ResetAll.cpp
                        api/service/GetAllSnapshot.cpp
                        api/service/RestoreSnapshot.cpp
                        api/service/CreateNewSnapshot.cpp
                        api/service/DeleteSnapshot.cpp
                        api/service/GetNodesForSnapshot.cpp
                        api/service/GetSnapshotDatasetsForNode.cpp
                        api/service/SetSnapshotDatasetsForNode.cpp
                        api/service/GetSnapshotForNode.cpp
                        api/service/SetVariable.cpp
                        api/service/RemoveVariable.cpp
                        api/service/GetVariable.cpp)

SET(cnd_src  ${cnd_src}	api/healt/HealtApiGroup.cpp
                        api/healt/ProcessHello.cpp
                        api/healt/ProcessBye.cpp)

SET(cnd_src  ${cnd_src}	api/unit_server/UnitServerApiGroup.cpp
                        api/unit_server/GetDescription.cpp
                        api/unit_server/LoadUnloadControlUnit.cpp
                        api/unit_server/NewUS.cpp
                        api/unit_server/DeleteUS.cpp
                        api/unit_server/ManageCUType.cpp
                        api/unit_server/GetSetFullUnitServer.cpp)

SET(cnd_src  ${cnd_src}	api/data_service/DataServiceApiGroup.cpp
                        api/data_service/AssociationControlUnit.cpp
                        api/data_service/GetAssociationByDS.cpp
                        api/data_service/GetAllDataService.cpp
                        api/data_service/GetBestEndpoints.cpp)

SET(cnd_src  ${cnd_src}	api/control_unit/SetInputDatasetAttributeValues.cpp
                        api/control_unit/ControlUnitGroup.cpp
                        api/control_unit/SetInstanceDescription.cpp
                        api/control_unit/SearchInstancesByUS.cpp
                        api/control_unit/GetInstance.cpp
                        api/control_unit/DeleteInstance.cpp
                        api/control_unit/Delete.cpp
                        api/control_unit/InitDeinit.cpp
                        api/control_unit/StartStop.cpp
                        api/control_unit/CopyInstance.cpp
                        api/control_unit/RecoverError.cpp
                        api/control_unit/GetCurrentDataset.cpp
						            api/control_unit/GetFullDescription.cpp
                        api/control_unit/SendStorageBurst.cpp)


SET(cnd_src  ${cnd_src}	api/node/NodeGroup.cpp
                        api/node/NodeCreateUnitServer.cpp
                        api/node/NodeGetDescription.cpp
                        api/node/NodeRegister.cpp
                        api/node/NodeHealthStatus.cpp
                        api/node/NodeLoadCompletion.cpp
                        api/node/NodeSearch.cpp
                        api/node/UpdateProperty.cpp
                        api/node/UpdatePropertyDefaultValues.cpp
                        api/node/GetPropertyDefaultValues.cpp
                        api/node/ChangeNodeState.cpp
                        api/node/CommandGet.cpp
                        api/node/CommandTemplateSet.cpp
                        api/node/CommandTemplateGet.cpp
                        api/node/CommandTemplateDelete.cpp
                        api/node/CommandTemplateSearch.cpp
                        api/node/CommandTemplateSubmit.cpp
                        api/node/CommandInstanceSubmit.cpp
                        api/node/CommandCommonUtility.cpp
                        api/node/KillCurrentCommand.cpp
                        api/node/ClearCommandQueue.cpp
                        api/node/ForwardNodeRpcMessage.cpp
			            api/node/GetPropertyDefaultValues.cpp
                        api/node/GetCommandAndTemplateDescription.cpp)

SET(cnd_src  ${cnd_src}	api/script/ScriptGroupApi.cpp
                        api/script/SearchScript.cpp
                        api/script/SaveScript.cpp
                        api/script/RemoveScript.cpp
                        api/script/LoadFullScript.cpp
                        api/script/SearchInstancesForScript.cpp
                        api/script/ManageScriptInstance.cpp
                        api/script/ExecutionPoolHeartbeat.cpp
                        api/script/UpdateScriptOnNode.cpp
                        api/script/UpdateBindType.cpp)

SET(cnd_src  ${cnd_src}	api/agent/AgentApiGroup.cpp
                        api/agent/LoadAgentDescription.cpp
                        api/agent/ListNodeForAgent.cpp
                        api/agent/GetAgentForNode.cpp
                        api/agent/SaveNodeAssociation.cpp
                        api/agent/LoadNodeAssociation.cpp
                        api/agent/RemoveNodeAssociation.cpp
                        api/agent/NodeOperation.cpp
                        api/agent/CheckAgentHostedProcess.cpp
                        api/agent/GetManagementConfiguration.cpp
                        api/agent/SetManagementConfiguration.cpp)

SET(cnd_src  ${cnd_src}	api/agent/logging/AgentLoggingApiGroup.cpp
                        api/agent/logging/GetProcessLogEntries.cpp
                        api/agent/logging/ManageNodeLogging.cpp)

#Worker
SET(cnd_src  ${cnd_src}	worker/DataWorker.cpp
                        worker/DeviceSharedDataWorker.cpp
                        worker/DeviceSharedDataWorkerMetric.cpp
                        worker/DeviceSharedDataWorkerMetricCollector.cpp)

#Persistence
SET(cnd_src  ${cnd_src} persistence/PersistenceManager.cpp
                        persistence/data_access/UnitServerDataAccess.cpp
                        persistence/data_access/NodeDataAccess.cpp
                        persistence/data_access/UtilityDataAccess.cpp
                        persistence/data_access/ControlUnitDataAccess.cpp
                        persistence/data_access/DataServiceDataAccess.cpp
                        persistence/data_access/SnapshotDataAccess.cpp
						persistence/data_access/TreeGroupDataAccess.cpp
                        persistence/data_access/LoggingDataAccess.cpp
						persistence/data_access/ScriptDataAccess.cpp
                        persistence/data_access/AgentDataAccess.cpp)

SET(cnd_src  ${cnd_src}	persistence/mongodb/MongoDBPersistenceDriver.cpp
                        persistence/mongodb/MongoDBUnitServerDataAccess.cpp
                        persistence/mongodb/MongoDBNodeDataAccess.cpp
                        persistence/mongodb/MongoDBUtilityDataAccess.cpp
                        persistence/mongodb/MongoDBControlUnitDataAccess.cpp
                        persistence/mongodb/MongoDBDataServiceDataAccess.cpp
                        persistence/mongodb/MongoDBSnapshotDataAccess.cpp
                        persistence/mongodb/MongoDBTreeGroupDataAccess.cpp
                        persistence/mongodb/MongoDBLoggingDataAccess.cpp
						persistence/mongodb/MongoDBScriptDataAccess.cpp
                        persistence/mongodb/MongoDBAgentDataAccess.cpp)
#Caching
SET(cnd_src  ${cnd_src}	cache_system/CacheDriver.cpp
                        cache_system/CacheDriverMetricCollector.cpp
                        cache_system/CacheDriverSharedMetricIO.cpp
                        cache_system/CouchbaseCacheDriver.cpp)


#Object Storage
SET(cnd_src  ${cnd_src}	object_storage/abstraction/ObjectStorageDataAccess.cpp)

SET(cnd_src  ${cnd_src}	object_storage/mongodb/MongoDBObjectStorageDataAccess.cpp
                        object_storage/mongodb/MongoDBObjectStorageDriver.cpp
                        object_storage/mongodb/ShardKeyManagement.cpp)

SET(cnd_src  ${cnd_src}	object_storage/mongodb_3/MongoDBObjectStorageDataAccess.cpp
                        object_storage/mongodb_3/NewMongoDBObjectStorageDriver.cpp)

SET(cnd_src  ${cnd_src}	batch/MDSBatchExecutor.cpp
                        batch/MDSBatchCommand.cpp)

SET(cnd_src  ${cnd_src}	batch/agent/AgentAckCommand.cpp
                        batch/agent/AgentProcessController.cpp
                        batch/agent/AgentCheckAgentProcess.cpp
                        batch/agent/AgentRemoveNodeSafety.cpp
                        batch/agent/AgentLoggingNodeManagement.cpp)

SET(cnd_src  ${cnd_src}	batch/node/UpdatePropertyCommand.cpp
                        batch/node/SubmitBatchCommand.cpp
                        batch/node/SendRpcCommand.cpp)

SET(cnd_src  ${cnd_src}	batch/unit_server/UnitServerAckBatchCommand.cpp
                        batch/unit_server/LoadUnloadControlUnit.cpp)

SET(cnd_src  ${cnd_src}	batch/control_unit/RegistrationAckBatchCommand.cpp
                        batch/control_unit/IDSTControlUnitBatchCommand.cpp
                        batch/control_unit/ApplyChangeSet.cpp
                        batch/control_unit/RecoverError.cpp)

SET(cnd_src  ${cnd_src}	batch/script/LoadInstanceOnUnitServer.cpp
                        batch/script/UpdateScriptOnNode.cpp)

SET(cnd_src  ${cnd_src}	batch/general/RestoreSnapshotBatch.cpp
                        batch/general/CreateSnapshotBatch.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${cnd_src})

FILE(GLOB files "../chaos_service_common/data/node/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_service_common/data/node/)


CheckConfigureBuild(couchbaseS libcouchbase:chaos-0.1 "-DLCB_NO_SSL=true;-DLCB_NO_TESTS=true;-DLCB_NO_TOOLS=true;-DLCB_NO_PLUGINS=true" https://github.com/amichelotti)

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

SET(BUILD_LIBRARY ${FrameworkLib})
SET(BUILD_LIBRARY ${BUILD_LIBRARY} chaos_common)
SET(BUILD_LIBRARY ${BUILD_LIBRARY} boost_random)
SET(BUILD_LIBRARY ${BUILD_LIBRARY} couchbaseS)
SET(BUILD_LIBRARY ${BUILD_LIBRARY} mongoclient)
IF(COMPILE_NEW_MONGODB_DRIVER)
  SET(BUILD_LIBRARY ${BUILD_LIBRARY} mongocxx)
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${BUILD_LIBRARY})
INSTALL_TARGETS(/bin ${PROJECT_NAME})
