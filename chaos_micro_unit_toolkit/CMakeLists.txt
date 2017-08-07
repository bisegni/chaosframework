cmake_minimum_required(VERSION 2.8)
project(chaos_micro_unit_toolkit)

INCLUDE_DIRECTORIES( ../)
INCLUDE_DIRECTORIES( SYSTEM .)

SET(source ChaosMicroUnitToolkit.cpp)
SET(${source} data/DataPack.cpp)
SET(${source} connection/ConnectionManager.cpp)
SET(${source} connection/protocol_adapter/AbstractProtocolAdapter.cpp
              connection/protocol_adapter/http/HTTPProtocolAdapter.cpp)
SET(${source} connection/unit_proxy/AbstractUnitProxy.cpp
              connection/unit_proxy/RawDriverUnitProxy.cpp)
SET(${source} external_lib/json.cpp
              external_lib/mongoose.cpp)

IF (BUILD_FORCE_STATIC)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${source})
ELSE ()
ADD_LIBRARY(${PROJECT_NAME} SHARED ${source})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0.0 SOVERSION 1)
ENDIF ()

TARGET_LINK_LIBRARIES(${PROJECT_NAME}  libc++)

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
FILE(GLOB files "external_lib/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolit/external_lib)

INSTALL(TARGETS chaos_micro_unit_toolkit
DESTINATION "lib"
PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
