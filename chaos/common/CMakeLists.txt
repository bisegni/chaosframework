project(chaos_common)
#### required libraries

IF(NOT CHAOS_DISABLE_EVENTFD)
  CheckConfigureBuild(event libevent:release-2.1.8-stable "--disable-openssl;--enable-static" https://github.com/amichelotti )
ELSE()
  SET(ZMQ_FLAGS "")
ENDIF()
CheckConfigureBuild(zmq zeromq4-1 "${ZMQ_FLAGS}" https://github.com/zeromq)

####

SET(common_lib_src chaos_errors.cpp
                   log/LogManager.cpp
                   log/ChaosLoggingBackend.cpp)

SET(common_lib_src ${common_lib_src}
                    additional_lib/mongoose.c
                    additional_lib/sqlite3.c
                    additional_lib/base64.cpp)

SET(common_lib_src ${common_lib_src}
                    metadata_logging/ErrorLoggingChannel.cpp
                    metadata_logging/AlarmLoggingChannel.cpp
                    metadata_logging/StandardLoggingChannel.cpp
                    metadata_logging/MetadataLoggingManager.cpp
                    metadata_logging/AbstractMetadataLogChannel.cpp
                    metadata_logging/BatchCommandLoggingChannel.cpp)

SET(common_lib_src ${common_lib_src}
        async_central/AsyncCentralManager.cpp
        async_central/AsyncPoolRunner.cpp
        async_central/TimerHandler.cpp)

SET(common_lib_src ${common_lib_src}
        healt_system/HealtManager.cpp
        healt_system/HealtMetric.cpp)

SET(common_lib_src ${common_lib_src}
        action/ActionExecutionSafetySystem.cpp
        action/EventAction.cpp
        action/ActionDescriptor.cpp
        action/DeclareAction.cpp
        action/DomainActions.cpp)

SET(common_lib_src ${common_lib_src}
        alarm/AlarmCatalog.cpp
        alarm/AlarmDescription.cpp
        alarm/MultiSeverityAlarm.cpp)

SET(common_lib_src ${common_lib_src}
        bson/bson/bcon.c
        bson/bson/bson-atomic.c
        bson/bson/bson-clock.c
        bson/bson/bson-context.c
        bson/bson/bson-decimal128.c
        bson/bson/bson-error.c
        bson/bson/bson-iso8601.c
        bson/bson/bson-iter.c
        bson/bson/bson-json.c
        bson/bson/bson-keys.c
        bson/bson/bson-md5.c
        bson/bson/bson-memory.c
        bson/bson/bson-oid.c
        bson/bson/bson-reader.c
        bson/bson/bson-string.c
        bson/bson/bson-timegm.c
        bson/bson/bson-utf8.c
        bson/bson/bson-value.c
        bson/bson/bson-writer.c
        bson/bson/bson.c
        bson/jsonsl/jsonsl.c)

SET(common_lib_src ${common_lib_src}
        configuration/GlobalConfiguration.cpp)

SET(common_lib_src ${common_lib_src}
        cronus_manager/CronusManager.cpp
        cronus_manager/CronJob.cpp)

SET(common_lib_src ${common_lib_src}
        metric/MetricCollector.cpp
        metric/MetricCollectorIO.cpp
        metric/AbstractMetricBackend.cpp
        metric/ConsoleMetricBackend.cpp
        metric/FileMetricBackend.cpp
        metric/CSVFileMetricBackend.cpp)

SET(common_lib_src ${common_lib_src}
        data/CDataWrapper.cpp
        data/CDataBuffer.cpp
        data/CDataVariant.cpp
        data/CUSchemaDB.cpp
        data/DatasetDB.cpp
        data/structured/Lists.cpp
        data/structured/Dataset.cpp
        data/structured/DatasetAttribute.cpp
        data/entity_db/EntityDB.cpp
        data/entity_db/sqlite_impl/SQLiteEntityDB.cpp
        data/entity/Entity.cpp data/broker/DataBroker.cpp
        data/broker/publisher/PublisherDataService.cpp
        data/broker/reader/ReaderDataService.cpp
        data/cache/FastHash.cpp
        data/cache/DataCache.cpp
        data/cache/LFDataCache.cpp)

SET(common_lib_src ${common_lib_src}
        debug/tracey.cpp)

SET(common_lib_src ${common_lib_src}
        data/cache/ChannelValueAccessor.cpp
        data/cache/KeyGroupCache.cpp
        data/cache/AttributeValue.cpp
        data/cache/AttributeCache.cpp
        data/cache/AbstractSharedDomainCache.cpp
        data/cache/AttributeValueSharedCache.cpp)

SET(common_lib_src ${common_lib_src}
        memory/ManagedMemory.cpp)

SET(common_lib_src ${common_lib_src}
        dispatcher/DefaultEventDispatcher.cpp
        dispatcher/EventTypeScheduler.cpp
        dispatcher/AbstractEventDispatcher.cpp
        dispatcher/AbstractCommandDispatcher.cpp
        dispatcher/DefaultCommandDispatcher.cpp
        dispatcher/DomainActionsScheduler.cpp
        dispatcher/AbstractActionScheduler.cpp
        dispatcher/SharedCommandDispatcher.cpp)

SET(common_lib_src ${common_lib_src}
        exception/CException.cpp
        exception/MetadataLoggingCException.cpp)

SET(common_lib_src ${common_lib_src}
        external_unit/ExternalUnitManager.cpp
        external_unit/AbstractServerAdapter.cpp
        external_unit/AbstractClientAdapter.cpp
        external_unit/ExternalUnitConnection.cpp
        external_unit/ExternalUnitClientEndpoint.cpp
        external_unit/ExternalUnitServerEndpoint.cpp
        external_unit/ExternalEchoEndpoint.cpp
        external_unit/ExternalUnitClientEndpoint.cpp
        external_unit/ExternalUnitServerEndpoint.cpp)

SET(common_lib_src ${common_lib_src}
        external_unit/serialization/AbstractExternalSerialization.cpp
        external_unit/serialization/ExternalBSONJsonSerialization.cpp
        external_unit/serialization/ExternalBSONExtJsonSerialization.cpp)

SET(common_lib_src ${common_lib_src}
        external_unit/http_adapter/HTTPServerAdapter.cpp
        external_unit/http_adapter/HTTPClientAdapter.cpp)

SET(common_lib_src ${common_lib_src}
        event/EventClient.cpp
        event/AsioImplEventClient
        event/AsioEventForwarder.cpp
        event/EventServer.cpp
        event/AsioImplEventServer.cpp
        event/AsioEventHandler.cpp
        event/evt_desc/EventDescriptor.cpp
        event/evt_desc/AlertEventDescriptor.cpp
        event/evt_desc/InstrumentEventDescriptor.cpp
        event/evt_desc/CommandEventDescriptor.cpp
        event/evt_desc/CustomEventDescriptor.cpp
        event/channel/EventChannel.cpp
        event/channel/AlertEventChannel.cpp
        event/channel/InstrumentEventChannel.cpp)

SET(common_lib_src ${common_lib_src}
        direct_io/DirectIOClient.cpp
        direct_io/DirectIOClientConnection.cpp
        direct_io/DirectIOServer.cpp
        direct_io/DirectIOURLManagment.cpp
        direct_io/DirectIOServerEndpoint.cpp
        direct_io/DirectIODispatcher.cpp
        direct_io/DirectIOPerformanceSession.cpp
        direct_io/impl/ZMQDirectIOClient.cpp
        direct_io/impl/ZMQDirectIOServer.cpp
        direct_io/impl/ZMQBaseClass.cpp
        direct_io/impl/ZMQDirectIOClientConnection.cpp
        direct_io/DirectIODispatcherMetricCollector.cpp
        direct_io/DirectIOClientConnectionMetricCollector.cpp
        direct_io/DirectIOClientConnectionSharedMetricIO.cpp
        direct_io/channel/DirectIOVirtualChannel.cpp
        direct_io/channel/DirectIOVirtualClientChannel.cpp
        direct_io/channel/DirectIOVirtualServerChannel.cpp
        direct_io/channel/DirectIODeviceClientChannel.cpp
        direct_io/channel/DirectIODeviceServerChannel.cpp
        direct_io/channel/DirectIOPerformanceServerChannel.cpp
        direct_io/channel/DirectIOPerformanceClientChannel.cpp
        direct_io/channel/DirectIOSystemAPIServerChannel.cpp
        direct_io/channel/DirectIOSystemAPIClientChannel.cpp)

SET(common_lib_src ${common_lib_src}
        io/IODataDriver.cpp
        io/IODirectIODriver.cpp
        io/ManagedDirectIODataDriver.cpp
        io/QueryCursor.cpp
        io/SharedManagedDirecIoDataDriver.cpp)

SET(common_lib_src ${common_lib_src}
        message/DeviceMessageChannel.cpp
        message/MDSMessageChannel.cpp
        message/MessageChannel.cpp
        message/MessageRequestFuture.cpp
        message/NodeMessageChannel.cpp
        message/MultiAddressMessageChannel.cpp
        message/MultiAddressMessageRequestFuture.cpp
        message/PerformanceNodeChannel.cpp
        message/MessageRequestDomain.cpp)

SET(common_lib_src ${common_lib_src}
        network/NetworkBroker.cpp
        network/PerformanceManagment.cpp
        network/URLServiceFeeder.cpp
        network/URLHAServiceFeeder.cpp
        network/URL.cpp)

SET(common_lib_src ${common_lib_src}
        rpc/RpcClient.cpp
        rpc/RpcServer.cpp
        rpc/RpcClientMetricCollector.cpp
        rpc/RpcServerMetricCollector.cpp
        rpc/zmq/ZMQClient.cpp
        rpc/zmq/ZMQServer.cpp)

SET(common_lib_src ${common_lib_src}
        utility/NamedService.cpp
        utility/StartableService.cpp
        utility/InizializableService.cpp
        utility/SWEService.cpp
        utility/CheckList.cpp
        utility/AggregatedCheckList.cpp
        utility/TimingUtil.cpp
        utility/AbstractListener.cpp
        utility/AbstractListenerContainer.cpp
        utility/ProcStat.cpp
	    utility/InetUtility.cpp
        utility/FSUtility.cpp)

SET(common_lib_src ${common_lib_src}
        batch_command/BatchCommandExecutor.cpp
        batch_command/BatchCommandSandbox.cpp
        batch_command/BatchCommand.cpp
        batch_command/BatchCommandDescription.cpp
        batch_command/AbstractSandbox.cpp
        batch_command/BatchCommandParallelSandbox.cpp)

SET(common_lib_src ${common_lib_src}
        plugin/PluginLoader.cpp
        plugin/PluginManager.cpp)

SET(common_lib_src ${common_lib_src}
        property/PropertyGroup.cpp
        property/PropertyCollector.cpp
        property/PropertyDescription.cpp)

SET(common_lib_src ${common_lib_src}
        script/ScriptManager.cpp
        script/AbstractScriptVM.cpp
        script/AbstractScriptableClass.cpp)

SET(common_lib_src ${common_lib_src}
        state_flag/StateFlagCatalog.cpp
        state_flag/StateFlag.cpp
        state_flag/DeclareStateFlag.cpp
        state_flag/StateFlagSDWrapper.cpp
        state_flag/StateFlagCatalogSDWrapper.cpp)

SET(common_lib_src ${common_lib_src}
        trigger/AbstractConsumer.cpp
        trigger/AbstractEvent.cpp
        trigger/AbstractSubject.cpp
        trigger/TriggerExecutionEnviroment.cpp
        trigger/trigger_types.cpp)


#compile lua chaos vm and core files
INCLUDE_DIRECTORIES(script/lua/core ./chaos/common ./chaos/common/metadata_logging ../../)
FILE(GLOB LuaCoreSource script/lua/core/*.c)
SET(common_lib_src ${common_lib_src}
	script/lua/LuaScriptVM.cpp
    script/lua/LuaModuleManager.cpp
	${LuaCoreSource})

IF (CLING_VIRTUAL_MACHINE)
    MESG("Add cling virtual machine compilation files")
    SET(common_lib_src ${common_lib_src}
        script/cling/CPPScriptVM.cpp)
ENDIF()

IF (NOT DEFINED BUILD_FORCE_STATIC)

ENDIF ()

#SET(common_lib_library $ENV{CHAOS_LINK_LIBRARY})
# separate_arguments(common_lib_library)

IF (BUILD_FORCE_STATIC)
  ADD_LIBRARY(${PROJECT_NAME} STATIC ${common_lib_src})
ELSE ()
  ADD_LIBRARY(${PROJECT_NAME} SHARED ${common_lib_src})
  set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0.0 SOVERSION 1)
ENDIF ()
#ADD_LIBRARY(${PROJECT_NAME} ${common_lib_src})
#SET_TARGET_PROPERTIES(chaos_common_static PROPERTIES OUTPUT_NAME chaos_common)

IF(CERN_ROOT)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${FrameworkLib} Tree Core)
ELSE()
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${FrameworkLib})
ENDIF()


FILE(GLOB files "external_unit/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/external_unit)
FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common)

FILE(GLOB files "alarm/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/alarm)

FILE(GLOB files "state_flag/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/state_flag)

FILE(GLOB files "metadata_logging/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/metadata_logging)
FILE(GLOB files "async_central/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/async_central)
FILE(GLOB files "batch_command/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/batch_command)
FILE(GLOB files "plugin/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/plugin)
FILE(GLOB files "action/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/action)
FILE(GLOB files "log/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/log)
FILE(GLOB files "bson/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/bson)
FILE(GLOB files "additional_lib/*.hpp")
INSTALL(FILES ${files} DESTINATION include/chaos/common/additional_lib)
FILE(GLOB files "bson/bson/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/bson/bson)
FILE(GLOB files "bson/util/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/bson/util)
FILE(GLOB files "bson/base/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/bson/base)
FILE(GLOB files "bson/platform/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/bson/platform)
FILE(GLOB files "configuration/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/configuration)
FILE(GLOB files "memory/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/memory)
FILE(GLOB files "data/*.h*")
INSTALL(FILES ${files} DESTINATION include/chaos/common/data)
FILE(GLOB files "data/cache/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/data/cache)
FILE(GLOB files "data/entity/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/data/entity)
FILE(GLOB files "data/entity_db/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/data/entity_db)
FILE(GLOB files "property/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/property)
FILE(GLOB files "dispatcher/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/dispatcher)
FILE(GLOB files "event/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/event)
FILE(GLOB files "event/channel/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/event/channel)
FILE(GLOB files "event/evt_desc/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/event/evt_desc)
FILE(GLOB files "exception/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/exception)
FILE(GLOB files "extension/*.hpp")
INSTALL(FILES ${files} DESTINATION include/chaos/common/extension)
FILE(GLOB files "extension/impl/*.hpp")
INSTALL(FILES ${files} DESTINATION include/chaos/common/extension/impl)
FILE(GLOB files "general/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/general)
FILE(GLOB files "io/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/io)
FILE(GLOB files "direct_io/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/direct_io)

FILE(GLOB files "direct_io/channel/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/direct_io/channel)

FILE(GLOB files "direct_io/impl/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/direct_io/impl)

FILE(GLOB files "message/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/message)
FILE(GLOB files "pool/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/pool)
FILE(GLOB files "pqueue/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/pqueue)
FILE(GLOB files "network/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/network)

FILE(GLOB files "rpc/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/rpc)
FILE(GLOB files "rpc/zmq/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/rpc/zmq)

FILE(GLOB files "sync_rpc/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/sync_rpc)

FILE(GLOB files "thread/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/thread)
FILE(GLOB files "utility/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/utility)

FILE(GLOB files "caching_system/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/caching_system)

FILE(GLOB files "caching_system/caching_thread/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/caching_system/caching_thread)
FILE(GLOB files "caching_system/caching_thread/tracker_interface/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/caching_system/caching_thread/tracker_interface)
FILE(GLOB files "caching_system/caching_thread/trackers/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/caching_system/caching_thread/trackers)

FILE(GLOB files "caching_system/common_buffer/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/caching_system/common_buffer)
FILE(GLOB files "caching_system/common_buffer/data_element/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/caching_system/common_buffer/data_element)
FILE(GLOB files "caching_system/common_buffer/helper/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/caching_system/common_buffer/helper)

FILE(GLOB files "script/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/script/)
FILE(GLOB files "script/lua/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/script/lua)
FILE(GLOB files "script/lua/core/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/common/script/lua/core)


INSTALL(TARGETS ${PROJECT_NAME}
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
# INSTALL(TARGETS chaos_common_static
#         DESTINATION "lib"
#         PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
