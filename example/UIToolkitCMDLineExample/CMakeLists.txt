SET(uitest_exe_src UIToolkitCMDLineExample.cpp)

find_package (Boost 1.39 REQUIRED
    COMPONENTS  program_options system thread chrono regex log log_setup)

find_library(MEMCACHED_LIB memcached PATHS /usr/lib,/usr/local/lib DOC "memcached library")
if(NOT MEMCACHED_LIB)
    message(FATAL_ERROR "Library memcached required, but not found!")
endif(NOT MEMCACHED_LIB)

find_library(MSGPACK_LIB msgpack PATHS /usr/lib,/usr/local/lib DOC "msgpack library")
if(NOT MSGPACK_LIB)
    message(FATAL_ERROR "Library msgpack required, but not found!")
endif(NOT MSGPACK_LIB)

find_library(MSGPACK_RPC_LIB msgpack-rpc PATHS /usr/lib,/usr/local/lib DOC "msgpack-rpc library")
if(NOT MSGPACK_RPC_LIB)
    message(FATAL_ERROR "Library msgpack-rpc required, but not found!")
endif(NOT MSGPACK_RPC_LIB)

find_library(MPIO_LIB mpio PATHS /usr/lib,/usr/local/lib DOC "mpio library")
if(NOT MPIO_LIB)
    message(FATAL_ERROR "Library mpio required, but not found!")
endif(NOT MPIO_LIB)

find_library(CHAOSCOMMON_LIB chaos_common PATHS /usr/lib,/usr/local/lib DOC "chaos_common library")
if(NOT CHAOSCOMMON_LIB)
    message(FATAL_ERROR "Library chaos_common required, but not found!")
endif(NOT CHAOSCOMMON_LIB)

find_library(CHAOSUI_LIB chaos_uitoolkit PATHS /usr/lib,/usr/local/lib DOC "chaos_uitoolkit library")
if(NOT CHAOSUI_LIB)
    message(FATAL_ERROR "Library chaos_cutoolkit required, but not found!")
endif(NOT CHAOSUI_LIB)


ADD_EXECUTABLE(UIToolkitTest ${uitest_exe_src})
TARGET_LINK_LIBRARIES(UIToolkitTest chaos_uitoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup memcached msgpack msgpack-rpc mpio) 
