SET(cnd_src main.cpp ChaosNodeDirectory.cpp)

ADD_EXECUTABLE(ChaosNodeDirectory ${cnd_src})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosNodeDirectory pthread dl chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosNodeDirectory chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


INSTALL_TARGETS(/bin ChaosNodeDirectory)