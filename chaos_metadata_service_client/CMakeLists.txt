cmake_minimum_required(VERSION 2.8)
project(chaos_metadata_service_client)

INCLUDE_DIRECTORIES(../)

SET(mcl_src ChaosMetadataServiceClient.cpp)

SET(mcl_src ${mcl_src}      api_proxy/ApiProxyManager.cpp
                            api_proxy/ApiProxy.cpp)

SET(mcl_src ${mcl_src}	    node_controller/CUController.cpp)

SET(mcl_src ${mcl_src}	    node_monitor/NodeMonitor.cpp
                            node_monitor/NodeFetcher.cpp
                            node_monitor/NodeController.cpp
                            node_monitor/ControlUnitController.cpp
                            node_monitor/NodeMonitorHandler.cpp
                            node_monitor/ControlUnitMonitorHandler.cpp)

SET(mcl_src ${mcl_src}	    event/EventDispatchManager.cpp
                            event/AbstractEventHandler.cpp)

SET(mcl_src ${mcl_src}	    event/alert/AlertEventHandler.cpp
                            event/alert/AlertLogEventHandler.cpp
                            event/alert/AlertAgentCheckProcessHandler.cpp)

SET(mcl_src ${mcl_src}      api_proxy/logging/SearchLogEntry.cpp
                            api_proxy/logging/GetLogForSourceUID.cpp
                            api_proxy/logging/GetLogDomainForSourceUID.cpp)


SET(mcl_src ${mcl_src}	    api_proxy/groups/AddNode.cpp
                            api_proxy/groups/DeleteNode.cpp
                            api_proxy/groups/GetDomains.cpp
                            api_proxy/groups/GetNodeChilds.cpp)

SET(mcl_src ${mcl_src}	    api_proxy/agent/ListNodeForAgent.cpp
                            api_proxy/agent/SaveNodeAssociation.cpp
                            api_proxy/agent/LoadNodeAssociation.cpp
                            api_proxy/agent/RemoveNodeAssociation.cpp
                            api_proxy/agent/LoadAgentDescription.cpp
                            api_proxy/agent/NodeOperation.cpp
                            api_proxy/agent/CheckAgentHostedProcess.cpp
                            api_proxy/agent/GetManagementConfiguration.cpp
                            api_proxy/agent/SetManagementConfiguration.cpp)

SET(mcl_src ${mcl_src}	    api_proxy/agent/logging/ManageNodeLogging.cpp
                            api_proxy/agent/logging/GetProcessLogEntries.cpp)

SET(mcl_src ${mcl_src}      api_proxy/service/ResetAll.cpp
                            api_proxy/service/CreateNewSnapshot.cpp
                            api_proxy/service/RestoreSnapshot.cpp
                            api_proxy/service/DeleteSnapshot.cpp
                            api_proxy/service/GetAllSnapshot.cpp
                            api_proxy/service/GetNodesForSnapshot.cpp
                            api_proxy/service/GetSnapshotForNode.cpp
                            api_proxy/service/GetSnapshotDatasetForNode.cpp
                            api_proxy/service/SetSnapshotDatasetsForNode.cpp
                            api_proxy/service/SetVariable.cpp
                            api_proxy/service/RemoveVariable.cpp
                            api_proxy/service/GetVariable.cpp)


SET(mcl_src ${mcl_src}      api_proxy/node/GetNodeDescription.cpp
                            api_proxy/node/NodeSearch.cpp
                            api_proxy/node/ChangeNodeState.cpp
                            api_proxy/node/CommandGet.cpp
                            api_proxy/node/CommandTemplateGet.cpp
                            api_proxy/node/CommandTemplateSet.cpp
                            api_proxy/node/CommandTemplateDelete.cpp
                            api_proxy/node/CommandTemplateSearch.cpp
                            api_proxy/node/CommandTemplateSubmit.cpp
                            api_proxy/node/CommandInstanceSubmit.cpp
                            api_proxy/node/UpdateProperty.cpp
			    api_proxy/node/GetPropertyDefaultValues.cpp
                            api_proxy/node/GetCommandAndTemplateDescription.cpp)

SET(mcl_src ${mcl_src}      api_proxy/data_service/NewDS.cpp
                            api_proxy/data_service/UpdateDS.cpp
                            api_proxy/data_service/DeleteDS.cpp
                            api_proxy/data_service/AssociationControlUnit.cpp
                            api_proxy/data_service/GetAssociationByDS.cpp
                            api_proxy/data_service/GetAllDS.cpp
                            api_proxy/data_service/GetBestEndpoints.cpp)

SET(mcl_src ${mcl_src}      api_proxy/control_unit/SetInputDatasetAttributeValues.cpp
                            api_proxy/control_unit/SetInstanceDescription.cpp
                            api_proxy/control_unit/SearchInstancesByUS.cpp
                            api_proxy/control_unit/GetInstance.cpp
                            api_proxy/control_unit/DeleteInstance.cpp
                            api_proxy/control_unit/Delete.cpp
                            api_proxy/control_unit/GetCurrentDataset.cpp
                            api_proxy/control_unit/InitDeinit.cpp
                            api_proxy/control_unit/StartStop.cpp
                            api_proxy/control_unit/CopyInstance.cpp
                            api_proxy/control_unit/RecoverError.cpp)

SET(mcl_src ${mcl_src}      api_proxy/unit_server/GetDescription.cpp
                            api_proxy/unit_server/NewUS.cpp
                            api_proxy/unit_server/DeleteUS.cpp
                            api_proxy/unit_server/LoadUnloadControlUnit.cpp
                            api_proxy/unit_server/ManageCUType.cpp
                            api_proxy/unit_server/GetSetFullUnitServer.cpp)

SET(mcl_src ${mcl_src}      api_proxy/script/SaveScript.cpp
                            api_proxy/script/RemoveScript.cpp
                            api_proxy/script/SearchScript.cpp
                            api_proxy/script/LoadFullScript.cpp
                            api_proxy/script/SearchInstancesForScript.cpp
                            api_proxy/script/ManageScriptInstance.cpp
                            api_proxy/script/UpdateScriptOnNode.cpp
                            api_proxy/script/UpdateBindType.cpp)

SET(mcl_src ${mcl_src}      monitor_system/MonitorManager.cpp
                            monitor_system/QuantumSlot.cpp
                            monitor_system/QuantumKeyConsumer.cpp
                            monitor_system/QuantumSlotScheduler.cpp
                            monitor_system/AbstractQuantumKeyAttributeHandler.cpp
                            monitor_system/QuantumTSTaggedAttributeHandler.cpp)

IF (BUILD_FORCE_STATIC)
  ADD_LIBRARY(${PROJECT_NAME} STATIC ${mcl_src})
ELSE ()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${mcl_src})
  set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0.0 SOVERSION 1)
ENDIF ()




TARGET_LINK_LIBRARIES(${PROJECT_NAME}  chaos_common ${FrameworkLib})

#install common header

FILE(GLOB files "../chaos_service_common/data/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_service_common/data)

FILE(GLOB files "../chaos_service_common/data/script/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_service_common/data/script)

FILE(GLOB files "../chaos_service_common/data/dataset/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_service_common/data/dataset)

FILE(GLOB files "../chaos_service_common/data/general/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_service_common/data/general)

#install lib header

FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client)

FILE(GLOB files "api_proxy/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/api_proxy)

FILE(GLOB files "api_proxy/logging/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/api_proxy/logging)

FILE(GLOB files "api_proxy/control_unit/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/api_proxy/control_unit)

FILE(GLOB files "api_proxy/unit_server/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/api_proxy/unit_server)

FILE(GLOB files "api_proxy/data_service/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/api_proxy/data_service)

FILE(GLOB files "api_proxy/node/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/api_proxy/node)

FILE(GLOB files "api_proxy/service/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/api_proxy/service)

FILE(GLOB files "api_proxy/groups/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/api_proxy/groups)
FILE(GLOB files "api_proxy/script/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/api_proxy/script)

FILE(GLOB files "monitor_system/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/monitor_system)

FILE(GLOB files "monitor_system/handler/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/monitor_system/handler)

FILE(GLOB files "node_controller/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/node_controller/)
FILE(GLOB files "event/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_metadata_service_client/event/)

INSTALL(TARGETS chaos_metadata_service_client
DESTINATION "lib"
PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
