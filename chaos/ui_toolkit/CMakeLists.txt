SET(ui_lib_src ChaosUIToolkit.cpp ChaosUIToolkitCWrapper.cc HighLevelApi/DeviceController.cpp HighLevelApi/HLDataApi.cpp HighLevelApi/HLInfrastructureApi.cpp LowLevelApi/LLDataApi.cpp LowLevelApi/LLRpcApi.cpp)

find_package (Boost 1.39 REQUIRED
    COMPONENTS  program_options system thread chrono regex log log_setup)

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