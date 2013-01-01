SET(uitest_exe_src UIToolkitCMDLineExample.cpp)

ADD_EXECUTABLE(UIToolkitTest ${uitest_exe_src})
TARGET_LINK_LIBRARIES(UIToolkitTest chaos_uitoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup memcached msgpack msgpack-rpc mpio) 
