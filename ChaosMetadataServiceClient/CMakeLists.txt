cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
INCLUDE_DIRECTORIES(../)

SET(mcl_src ChaosMetadataServiceClient.cpp)

SET(mcl_src ${mcl_src}      api_proxy/ApiProxyManager.cpp
                            api_proxy/ApiProxy.cpp)

SET(mcl_src ${mcl_src}      api_proxy/service/ResetAll.cpp)

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
                            api_proxy/control_unit/GetCurrentDataset.cpp
                            api_proxy/control_unit/InitDeinit.cpp
                            api_proxy/control_unit/StartStop.cpp)

SET(mcl_src ${mcl_src}      api_proxy/unit_server/GetDescription.cpp
                            api_proxy/unit_server/NewUS.cpp
                            api_proxy/unit_server/LoadUnloadControlUnit.cpp
                            api_proxy/unit_server/ManageCUType.cpp)

SET(mcl_src ${mcl_src}      monitor_system/MonitorManager.cpp
                            monitor_system/QuantumSlot.cpp
                            monitor_system/QuantumKeyConsumer.cpp
                            monitor_system/QuantumSlotScheduler.cpp
                            monitor_system/AbstractQuantumKeyAttributeHandler.cpp
                            monitor_system/QuantumTSTaggedAttributeHandler.cpp)

IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(chaos_metadata_service_client STATIC ${mcl_src})
ELSE()
ADD_LIBRARY(chaos_metadata_service_client SHARED ${mcl_src})
ENDIF()


TARGET_LINK_LIBRARIES(chaos_metadata_service_client  chaos_common ${FrameworkLib})

FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/ChaosMetadataServiceClient)

FILE(GLOB files "api_proxy/*.h")
INSTALL(FILES ${files} DESTINATION include/ChaosMetadataServiceClient/api_proxy)

FILE(GLOB files "api_proxy/control_unit/*.h")
INSTALL(FILES ${files} DESTINATION include/ChaosMetadataServiceClient/api_proxy/control_unit)

FILE(GLOB files "api_proxy/unit_server/*.h")
INSTALL(FILES ${files} DESTINATION include/ChaosMetadataServiceClient/api_proxy/unit_server)

FILE(GLOB files "api_proxy/data_service/*.h")
INSTALL(FILES ${files} DESTINATION include/ChaosMetadataServiceClient/api_proxy/data_service)

FILE(GLOB files "api_proxy/node/*.h")
INSTALL(FILES ${files} DESTINATION include/ChaosMetadataServiceClient/api_proxy/node)

FILE(GLOB files "api_proxy/service/*.h")
INSTALL(FILES ${files} DESTINATION include/ChaosMetadataServiceClient/api_proxy/service)

FILE(GLOB files "monitor_system/*.h")
INSTALL(FILES ${files} DESTINATION include/ChaosMetadataServiceClient/monitor_system)

FILE(GLOB files "monitor_system/handler/*.h")
INSTALL(FILES ${files} DESTINATION include/ChaosMetadataServiceClient/monitor_system/handler)

INSTALL(TARGETS chaos_metadata_service_client
DESTINATION "lib"
PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
