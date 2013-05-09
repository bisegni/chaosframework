SET(cnd_src main.cpp)

ADD_EXECUTABLE(ChaosHSTService ${cnd_src})
TARGET_LINK_LIBRARIES(ChaosHSTService chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio) 

INSTALL_TARGETS(/bin ChaosHSTService)