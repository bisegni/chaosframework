project(chaos_cutoolkit)
INCLUDE_DIRECTORIES(chaos/cu ../../)
SET(cu_lib_src
        ChaosCUToolkit.cpp
        command_manager/CommandManager.cpp)

SET(cu_lib_src ${cu_lib_src}
        additional_lib/mongoose.c)

SET(cu_lib_src ${cu_lib_src}
        external_gateway/ExternalUnitGateway.cpp)

SET(cu_lib_src ${cu_lib_src}
        driver_manager/DriverManager.cpp
        driver_manager/driver/AbstractDriver.cpp
        driver_manager/driver/DriverAccessor.cpp
        driver_manager/driver/DriverPluginLoader.cpp
        driver_manager/driver/AbstractDriverPlugin.cpp)

SET(cu_lib_src ${cu_lib_src}
        driver_manager/driver/BasicIODriver.cpp
        driver_manager/driver/BasicIODriverInterface.cpp)

SET(cu_lib_src ${cu_lib_src}
        data_manager/DataManager.cpp
        data_manager/KeyDataStorage.cpp
        data_manager/DataBrokerManager.cpp
        data_manager/DataBroker.cpp
        data_manager/data_manager_types.cpp
        data_manager/trigger_system/EventManager.cpp
        data_manager/trigger_system/dataset_event/DatasetConsumer.cpp
        data_manager/trigger_system/dataset_event/DatasetEvent.cpp
        data_manager/trigger_system/dataset_event/DatasetSubject.cpp
        data_manager/trigger_system/dataset_event/RegisterEnviroment.cpp
        data_manager/publishing/publishing_types.cpp
        data_manager/publishing/PublishTarget.cpp
        data_manager/publishing/PublishingManager.cpp
        data_manager/manipulation/DataBrokerEditor.cpp
        data_manager/manipulation/DatasetCacheWrapper.cpp
        data_manager/manipulation/DatasetEditor.cpp)

SET(cu_lib_src ${cu_lib_src}
        control_manager/script/ScriptableExecutionUnit.cpp
        control_manager/script/api/EUDSValueManagement.cpp
        control_manager/script/api/EULiveManagment.cpp
	control_manager/script/api/EUSearch.cpp
	control_manager/script/api/plugin/EUPluginApiWrapper.cpp
	control_manager/script/api/plugin/EUPluginLoader.cpp
	control_manager/script/ScriptableExecutionUnit.cpp
	)
	

SET(cu_lib_src ${cu_lib_src}
        control_manager/AbstractControlUnit.cpp
        control_manager/ControlManager.cpp
        control_manager/SCAbstractControlUnit.cpp
        control_manager/RTAbstractControlUnit.cpp
        control_manager/AbstractExecutionUnit.cpp
        control_manager/WorkUnitManagement.cpp
        control_manager/IOCU.cpp
        control_manager/handler/AbstractAttributeHandler.cpp
        control_manager/handler/DatasetAttributeHandler.cpp
        control_manager/slow_command/SlowCommandExecutor.cpp
        control_manager/slow_command/SlowCommand.cpp
        control_manager/slow_command/command/SCWaitCommand.cpp
        control_manager/AttributeSharedCacheWrapper.cpp
        control_manager/ProxyControlUnit.cpp
        control_manager/ControlUnitApiInterface.cpp)

SET(cu_lib_src ${cu_lib_src}
        control_manager/execution_pool/ExecutionPool.cpp)
 IF (BUILD_FORCE_STATIC)
     ADD_LIBRARY(${PROJECT_NAME} STATIC ${cu_lib_src})
 ELSE ()
     ADD_LIBRARY(${PROJECT_NAME} SHARED ${cu_lib_src})
     set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0.0 SOVERSION 1)
 ENDIF ()

#ADD_LIBRARY(${PROJECT_NAME} ${cu_lib_src})
#SET_TARGET_PROPERTIES(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})


SET(chaos_cutoolkit_lib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(chaos_cutoolkit_lib)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_common ${chaos_cutoolkit_lib})

FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu)

FILE(GLOB files "driver_manager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/driver_manager)

FILE(GLOB files "driver_manager/driver/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/driver_manager/driver)

FILE(GLOB files "command_manager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/command_manager)

FILE(GLOB files "ConfigurationManager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/ConfigurationManager)

FILE(GLOB files "control_manager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/control_manager)

FILE(GLOB files "control_manager/handler/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/control_manager/handler)

FILE(GLOB files "control_manager/slow_command/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/control_manager/slow_command)
FILE(GLOB files "control_manager/execution_pool/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/control_manager/execution_pool)

#we don't install default command header

FILE(GLOB files "data_manager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/data_manager)

INSTALL(TARGETS ${PROJECT_NAME}
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
