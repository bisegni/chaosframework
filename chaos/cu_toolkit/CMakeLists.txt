SET(cu_lib_src  ChaosCUToolkit.cpp CommandManager/CommandManager.cpp)
SET(cu_lib_src  ${cu_lib_src} driver_manager/DriverManager.cpp driver_manager/driver/AbstractDriver.cpp driver_manager/driver/DriverAccessor.cpp driver_manager/driver/DriverPluginLoader.cpp driver_manager/driver/AbstractDriverPlugin.cpp)
SET(cu_lib_src  ${cu_lib_src} DataManager/DataManager.cpp DataManager/KeyDataStorage.cpp DataManager/MultiBufferDataStorage.cpp DataManager/DataBuffer/OutputDataBuffer.cpp)
SET(cu_lib_src  ${cu_lib_src} ControlManager/DSAttributeHandlerExecutionEngine.cpp ControlManager/handler/DSAttributeHandler.cpp ControlManager/AbstractControlUnit.cpp ControlManager/ControlManager.cpp ControlManager/SCAbstractControlUnit.cpp ControlManager/RTAbstractControlUnit.cpp)
SET(cu_lib_src  ${cu_lib_src} ControlManager/slow_command/SlowCommandExecutor.cpp ControlManager/slow_command/SlowCommand.cpp ControlManager/slow_command/command/SetAttributeCommand.cpp)


ADD_LIBRARY(chaos_cutoolkit SHARED ${cu_lib_src})

set_target_properties(chaos_cutoolkit PROPERTIES VERSION 1.0.0 SOVERSION 1) 

TARGET_LINK_LIBRARIES(chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio) 

FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit)

FILE(GLOB files "driver_manager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/driver_manager)

FILE(GLOB files "driver_manager/driver/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/driver_manager/driver)

FILE(GLOB files "CommandManager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/CommandManager)

FILE(GLOB files "ConfigurationManager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/ConfigurationManager)

FILE(GLOB files "ControlManager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/ControlManager)

FILE(GLOB files "ControlManager/handler/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/ControlManager/handler)

FILE(GLOB files "ControlManager/slow_command/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/ControlManager/slow_command)

#we don't install default command header

FILE(GLOB files "DataManager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/DataManager)

FILE(GLOB files "DataManager/DataBuffer/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/DataManager/DataBuffer)

INSTALL(TARGETS chaos_cutoolkit
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

