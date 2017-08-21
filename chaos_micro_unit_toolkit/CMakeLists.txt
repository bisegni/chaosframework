cmake_minimum_required(VERSION 2.8)

project(chaos_micro_unit_toolkit)

INCLUDE_DIRECTORIES( ../)

SET(source ChaosMicroUnitToolkit.cpp)
SET(source ${source} data/DataPack.cpp)
SET(source ${source} connection/ConnectionManager.cpp)
SET(source ${source} connection/protocol_adapter/AbstractProtocolAdapter.cpp
                     connection/protocol_adapter/http/HTTPProtocolAdapter.cpp)
SET(source ${source} connection/unit_proxy/AbstractUnitProxy.cpp
                     connection/unit_proxy/UnitProxyHandlerWrapper.cpp)
SET(source ${source} connection/unit_proxy/raw_driver/RawDriverUnitProxy.cpp
                     connection/unit_proxy/raw_driver/RawDriverHandlerWrapper.cpp)
SET(source ${source} external_lib/jsoncpp.cpp
                     external_lib/mongoose.c
                     external_lib/base64.cpp)

IF (BUILD_FORCE_STATIC)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${source})
ELSE ()
ADD_LIBRARY(${PROJECT_NAME} SHARED ${source})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0.0 SOVERSION 1)
ENDIF ()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} pthread)

FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolit)
FILE(GLOB files "connection/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolit/connection)
FILE(GLOB files "connection/protocol_adapter/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolit/connection/protocol_adapter)
FILE(GLOB files "connection/protocol_adapter/http/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolit/connection/protocol_adapter/http)
FILE(GLOB files "connection/unit_proxy/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolit/connection/unit_proxy)
FILE(GLOB files "connection/unit_proxy/raw_driver/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolit/connection/unit_proxy/raw_driver)
FILE(GLOB files "external_lib/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolit/external_lib)

INSTALL(TARGETS chaos_micro_unit_toolkit
DESTINATION "lib"
PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
