SET(uicwrapper_example_exe_src UIToolkitCMDLineCWapperExample.c)

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

find_library(CHAOS_UITOOLKIT_LIP chaos_uitoolkit PATHS /usr/lib,/usr/local/lib DOC "CHAOS UIToolkit library")
if(NOT CHAOS_UITOOLKIT_LIP)
    message(FATAL_ERROR "Library chaos_uitoolkit required, but not found!")
endif(NOT CHAOS_UITOOLKIT_LIP)

ADD_EXECUTABLE(UICWrapperToolkitTest ${uicwrapper_example_exe_src})
TARGET_LINK_LIBRARIES(UICWrapperToolkitTest chaos_uitoolkit) 
