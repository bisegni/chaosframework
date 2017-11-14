cmake_minimum_required(VERSION 2.8)

project(chaos_micro_unit_toolkit)

INCLUDE_DIRECTORIES( ../)

SET(source ChaosMicroUnitToolkit.cpp)
SET(source ${source} data/DataPack.cpp)
SET(source ${source} connection/ConnectionManager.cpp)
SET(source ${source} connection/connection_adapter/AbstractConnectionAdapter.cpp
                     connection/connection_adapter/http/HTTPConnectionAdapter.cpp)
SET(source ${source} connection/unit_proxy/AbstractUnitProxy.cpp
                     connection/unit_proxy/UnitProxyHandlerWrapper.cpp)
SET(source ${source} connection/unit_proxy/raw_driver/RawDriverUnitProxy.cpp
                     connection/unit_proxy/raw_driver/RawDriverHandlerWrapper.cpp)
SET(source ${source}
                    external_lib/bson/bson/bson.c
                    external_lib/bson/bson/bson-atomic.c
                    external_lib/bson/bson/bson-clock.c
                    external_lib/bson/bson/bson-context.c
                    external_lib/bson/bson/bson-decimal128.c
                    external_lib/bson/bson/bson-error.c
                    external_lib/bson/bson/bson-iso8601.c
                    external_lib/bson/bson/bson-iter.c
                    external_lib/bson/bson/bson-json.c
                    external_lib/bson/bson/bson-keys.c
                    external_lib/bson/bson/bson-md5.c
                    external_lib/bson/bson/bson-memory.c
                    external_lib/bson/bson/bson-oid.c
                    external_lib/bson/bson/bson-reader.c
                    external_lib/bson/bson/bson-string.c
                    external_lib/bson/bson/bson-timegm.c
                    external_lib/bson/bson/bson-utf8.c
                    external_lib/bson/bson/bson-value.c
                    external_lib/bson/bson/bson-writer.c
                    external_lib/bson/bson/bson.c
                    external_lib/jsonsl/jsonsl.c)
SET(source ${source} external_lib/mongoose.c
                     external_lib/base64.cpp)

IF (BUILD_FORCE_STATIC)
ADD_LIBRARY(${PROJECT_NAME} STATIC ${source})
ELSE ()
ADD_LIBRARY(${PROJECT_NAME} SHARED ${source})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0.0 SOVERSION 1)
ENDIF ()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} pthread)

FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolkit)
FILE(GLOB files "connection/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolkit/connection)
FILE(GLOB files "connection/connection_adapter/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolkit/connection/connection_adapter)
FILE(GLOB files "connection/connection_adapter/http/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolkit/connection/connection_adapter/http)
FILE(GLOB files "connection/unit_proxy/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolkit/connection/unit_proxy)
FILE(GLOB files "connection/unit_proxy/raw_driver/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolkit/connection/unit_proxy/raw_driver)
FILE(GLOB files "external_lib/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos_micro_unit_toolkit/external_lib)

INSTALL(TARGETS chaos_micro_unit_toolkit
DESTINATION "lib"
PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
