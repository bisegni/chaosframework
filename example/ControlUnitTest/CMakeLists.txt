SET(cutest_exe_src ControlUnitExample.cpp WorkerCU.cpp)

ADD_EXECUTABLE(ControlUnitTest ${cutest_exe_src})
TARGET_LINK_LIBRARIES(ControlUnitTest chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup memcached msgpack msgpack-rpc mpio) 
