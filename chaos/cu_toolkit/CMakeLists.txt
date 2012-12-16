SET(cu_lib_src  ChaosCUToolkit.cpp CommandManager/CommandManager.cpp ConfigurationManager/ConfigurationManager.cpp ControlManager/DSAttributeHandlerExecutionEngine.cpp 
                ControlManager/handler/DSAttributeHandler.cpp ControlManager/AbstractControlUnit.cpp ControlManager/ControlManager.cpp DataManager/DataManager.cpp 
                DataManager/KeyDataStorage.cpp DataManager/MultiBufferDataStorage.cpp DataManager/DataBuffer/OutputDataBuffer.cpp ) 

find_library(BOOST_PO boost_program_options PATHS /usr/lib,/usr/local/lib,usr/local/lib DOC "boost_program_options library")
if(NOT BOOST_PO)
    message(FATAL_ERROR "Library boost_program_options required, but not found!")
endif(NOT BOOST_PO)

find_library(BOOST_SY boost_system PATHS /usr/lib,/usr/local/lib,usr/local/lib DOC "boost_system library")
if(NOT BOOST_SY)
    message(FATAL_ERROR "Library boost_system required, but not found!")
endif(NOT BOOST_SY)

find_library(BOOST_TH boost_thread PATHS /usr/lib,/usr/local/lib,usr/local/lib DOC "boost_thread library")
if(NOT BOOST_TH)
    message(FATAL_ERROR "Library boost_thread required, but not found!")
endif(NOT BOOST_TH)

find_library(BOOST_CH boost_chrono PATHS /usr/lib,/usr/local/lib,usr/local/lib DOC "boost_chrono library")
if(NOT BOOST_CH)
    message(FATAL_ERROR "Library boost_chrono required, but not found!")
endif(NOT BOOST_CH)

find_library(BOOST_RE boost_regex PATHS /usr/lib,/usr/local/lib,usr/local/lib DOC "boost_regex library")
if(NOT BOOST_RE)
    message(FATAL_ERROR "Library boost_regex required, but not found!")
endif(NOT BOOST_RE)

find_library(BOOST_LOG boost_log PATHS /usr/lib,/usr/local/lib,usr/local/lib DOC "boost_log library")
if(NOT BOOST_LOG)
    message(FATAL_ERROR "Library boost_log required, but not found!")
endif(NOT BOOST_LOG)

find_library(BOOST_LOG_SET boost_log_setup PATHS /usr/lib,/usr/local/lib,usr/local/lib DOC "boost_log_setup library")
if(NOT BOOST_LOG_SET)
    message(FATAL_ERROR "Library boost_log_setup required, but not found!")
endif(NOT BOOST_LOG_SET)
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

FILE(GLOB files "ControlManager/handler/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/ControlManager/handler)

FILE(GLOB files "DataManager/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/DataManager)

FILE(GLOB files "DataManager/DataBuffer/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/cu_toolkit/DataManager/DataBuffer)

INSTALL(TARGETS chaos_cutoolkit
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE GROUP_READ WORLD_READ)

