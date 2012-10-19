SET(cu_lib_src ChaosCUToolkit.cpp CommandManager/CommandManager.cpp ConfigurationManager/ConfigurationManager.cpp ControlManager/AbstractControlUnit.cpp ControlManager/ControlManager.cpp DataManager/DataManager.cpp DataManager/KeyDataStorage.cpp DataManager/MultiBufferDataStorage.cpp DataManager/DataBuffer/OutputDataBuffer.cpp) 

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

ADD_LIBRARY(chaos_cutoolkit SHARED ${cu_lib_src})
TARGET_LINK_LIBRARIES(chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup memcached msgpack msgpack-rpc mpio) 

FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit)

FILE(GLOB files "CommandManager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/CommandManager)

FILE(GLOB files "ConfigurationManager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/ConfigurationManager)

FILE(GLOB files "ControlManager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/ControlManager)

FILE(GLOB files "DataManager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/DataManager)

FILE(GLOB files "DataManager/DataBuffer/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/DataManager/DataBuffer)

INSTALL(TARGETS chaos_cutoolkit
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE GROUP_READ WORLD_READ)

