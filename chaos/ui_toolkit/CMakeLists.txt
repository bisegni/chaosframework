SET(ui_lib_src ChaosUIToolkit.cpp ChaosUIToolkitCWrapper.cc HighLevelApi/DeviceController.cpp HighLevelApi/HLDataApi.cpp HighLevelApi/HLInfrastructureApi.cpp LowLevelApi/LLDataApi.cpp LowLevelApi/LLRpcApi.cpp)

find_package (Boost 1.39 REQUIRED
    COMPONENTS  program_options system thread chrono regex log log_setup)

find_library(MEMCACHED_LIB memcached PATHS /usr/lib,/usr/local/lib,usr/local/lib  DOC "memcached library")
if(NOT MEMCACHED_LIB)
    message(FATAL_ERROR "Library memcached required, but not found!")
endif(NOT MEMCACHED_LIB)

find_library(MSGPACK_LIB msgpack PATHS /usr/lib,/usr/local/lib,usr/local/lib  DOC "msgpack library")
if(NOT MSGPACK_LIB)
    message(FATAL_ERROR "Library msgpack required, but not found!")
endif(NOT MSGPACK_LIB)

find_library(MSGPACK_RPC_LIB msgpack-rpc PATHS /usr/lib,/usr/local/lib,usr/local/lib  DOC "msgpack-rpc library")
if(NOT MSGPACK_RPC_LIB)
    message(FATAL_ERROR "Library msgpack-rpc required, but not found!")
endif(NOT MSGPACK_RPC_LIB)

find_library(MPIO_LIB mpio PATHS /usr/lib,/usr/local/lib,usr/local/lib  DOC "mpio library")
if(NOT MPIO_LIB)
    message(FATAL_ERROR "Library mpio required, but not found!")
endif(NOT MPIO_LIB)

ADD_LIBRARY(chaos_uitoolkit SHARED ${ui_lib_src})
TARGET_LINK_LIBRARIES(chaos_uitoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup memcached msgpack msgpack-rpc mpio) 

FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit)

FILE(GLOB files "Common/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit/Common)

FILE(GLOB files "HighLevelApi/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit/HighLevelApi)

FILE(GLOB files "LowLevelApi/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit/LowLevelApi)

INSTALL(TARGETS chaos_uitoolkit
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE GROUP_READ WORLD_READ)