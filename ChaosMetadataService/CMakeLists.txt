cmake_minimum_required(VERSION 2.8)
project(ChaosMetadataService)
IF (DEFINED ENV{CHAOS_BUNDLE} )
  include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
ENDIF()

INCLUDE_DIRECTORIES(chaos/common ../)

SET(cnd_src				main.cpp
						ChaosMetadataService.cpp)

SET(cnd_src  ${cnd_src} ../chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.cpp
                        ../chaos_service_common/persistence/mongodb/MongoDBAccessor.cpp)

SET(cnd_src  ${cnd_src} ../chaos_service_common/persistence/data_access/AbstractDataAccess.cpp
						../chaos_service_common/persistence/data_access/AbstractPersistenceDriver.cpp)

SET(cnd_src  ${cnd_src}	cron_job/MDSCronousManager.cpp
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
                        api/service/GetSnapshotForNode.cpp)

SET(cnd_src  ${cnd_src}	api/healt/HealtApiGroup.cpp
                        api/healt/ProcessHello.cpp
                        api/healt/ProcessBye.cpp)

SET(cnd_src  ${cnd_src}	api/unit_server/UnitServerApiGroup.cpp
                        api/unit_server/GetDescription.cpp
                        api/unit_server/LoadUnloadControlUnit.cpp
                        api/unit_server/NewUS.cpp
                        api/unit_server/DeleteUS.cpp
                        api/unit_server/ManageCUType.cpp)

SET(cnd_src  ${cnd_src}	api/data_service/DataServiceApiGroup.cpp
                        api/data_service/NewDS.cpp
                        api/data_service/UpdateDS.cpp
                        api/data_service/DeleteDS.cpp
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
						api/control_unit/GetFullDescription.cpp)

SET(cnd_src  ${cnd_src}	api/node/NodeGroup.cpp
                        api/node/NodeCreateUnitServer.cpp
                        api/node/NodeGetDescription.cpp
                        api/node/NodeRegister.cpp
                        api/node/NodeLoadCompletion.cpp
                        api/node/NodeSearch.cpp
                        api/node/UpdateProperty.cpp
                        api/node/ChangeNodeState.cpp
                        api/node/CommandGet.cpp
                        api/node/CommandTemplateSet.cpp
                        api/node/CommandTemplateGet.cpp
                        api/node/CommandTemplateDelete.cpp
                        api/node/CommandTemplateSearch.cpp
                        api/node/CommandTemplateSubmit.cpp
                        api/node/CommandInstanceSubmit.cpp
                        api/node/CommandCommonUtility.cpp
                        api/node/GetCommandAndTemplateDescription.cpp)

SET(cnd_src  ${cnd_src}	api/script/ScriptGroupApi.cpp
                        api/script/SearchScript.cpp
                        api/script/SaveScript.cpp
                        api/script/LoadFullScript.cpp
                        api/script/SearchInstancesForScript.cpp
                        api/script/ManageScriptInstance.cpp
                        api/script/ExecutionPoolHeartbeat.cpp)

SET(cnd_src  ${cnd_src}	persistence/data_access/UnitServerDataAccess.cpp
                        persistence/data_access/NodeDataAccess.cpp
                        persistence/data_access/UtilityDataAccess.cpp
                        persistence/data_access/ControlUnitDataAccess.cpp
                        persistence/data_access/DataServiceDataAccess.cpp
                        persistence/data_access/SnapshotDataAccess.cpp
						persistence/data_access/TreeGroupDataAccess.cpp
                        persistence/data_access/LoggingDataAccess.cpp
						persistence/data_access/ScriptDataAccess.cpp)

SET(cnd_src  ${cnd_src}	persistence/mongodb/MongoDBPersistenceDriver.cpp
                        persistence/mongodb/MongoDBUnitServerDataAccess.cpp
                        persistence/mongodb/MongoDBNodeDataAccess.cpp
                        persistence/mongodb/MongoDBUtilityDataAccess.cpp
                        persistence/mongodb/MongoDBControlUnitDataAccess.cpp
                        persistence/mongodb/MongoDBDataServiceDataAccess.cpp
                        persistence/mongodb/MongoDBSnapshotDataAccess.cpp
                        persistence/mongodb/MongoDBTreeGroupDataAccess.cpp
                        persistence/mongodb/MongoDBLoggingDataAccess.cpp
						persistence/mongodb/MongoDBScriptDataAccess.cpp)

SET(cnd_src  ${cnd_src}	batch/MDSBatchExecutor.cpp
                        batch/MDSBatchCommand.cpp)

SET(cnd_src  ${cnd_src}	batch/node/UpdatePropertyCommand.cpp
                        batch/node/SubmitBatchCommand.cpp)

SET(cnd_src  ${cnd_src}	batch/unit_server/UnitServerAckBatchCommand.cpp
                        batch/unit_server/LoadUnloadControlUnit.cpp)

SET(cnd_src  ${cnd_src}	batch/control_unit/RegistrationAckBatchCommand.cpp
                        batch/control_unit/IDSTControlUnitBatchCommand.cpp
                        batch/control_unit/ApplyChangeSet.cpp
                        batch/control_unit/RecoverError.cpp)

SET(cnd_src  ${cnd_src}	batch/script/LoadInstanceOnUnitServer.cpp)

SET(cnd_src  ${cnd_src}	batch/general/RestoreSnapshotBatch.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${cnd_src})

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_common mongoclient ${FrameworkLib})
INSTALL_TARGETS(/bin ${PROJECT_NAME})
