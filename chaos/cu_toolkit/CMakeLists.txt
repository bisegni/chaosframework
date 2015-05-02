SET(cu_lib_src  ChaosCUToolkit.cpp CommandManager/CommandManager.cpp)

SET(cu_lib_src  ${cu_lib_src} driver_manager/DriverManager.cpp driver_manager/driver/AbstractDriver.cpp driver_manager/driver/DriverAccessor.cpp                driver_manager/driver/DriverPluginLoader.cpp driver_manager/driver/AbstractDriverPlugin.cpp)
SET(cu_lib_src  ${cu_lib_src} driver_manager/driver/BasicIODriver.cpp driver_manager/driver/BasicIODriverInterface.cpp)

SET(cu_lib_src  ${cu_lib_src} DataManager/DataManager.cpp DataManager/KeyDataStorage.cpp)
SET(cu_lib_src  ${cu_lib_src} ControlManager/AbstractControlUnit.cpp ControlManager/ControlManager.cpp)
SET(cu_lib_src  ${cu_lib_src} ControlManager/SCAbstractControlUnit.cpp ControlManager/RTAbstractControlUnit.cpp ControlManager/WorkUnitManagement.cpp)
SET(cu_lib_src  ${cu_lib_src} ControlManager/IOCU.cpp)

SET(cu_lib_src  ${cu_lib_src} ControlManager/slow_command/SlowCommandExecutor.cpp ControlManager/slow_command/SlowCommand.cpp ControlManager/slow_command/command/SetAttributeCommand.cpp
								ControlManager/AttributeSharedCacheWrapper.cpp)

IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(chaos_cutoolkit STATIC ${cu_lib_src})
ELSE()
ADD_LIBRARY(chaos_cutoolkit SHARED ${cu_lib_src})
ENDIF()

#SET_TARGET_PROPERTIES(chaos_cutoolkit_static PROPERTIES OUTPUT_NAME chaos_cutoolkit)
#set_target_properties(chaos_cutoolkit PROPERTIES VERSION 1.0.0 SOVERSION 1) 

SET(chaos_cutoolkit_lib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(chaos_cutoolkit_lib)
TARGET_LINK_LIBRARIES(chaos_cutoolkit  chaos_common ${chaos_cutoolkit_lib})

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

# INSTALL(TARGETS chaos_cutoolkit_static
#         DESTINATION "lib"
#         PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

