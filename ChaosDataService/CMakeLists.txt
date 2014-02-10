SET(cnd_src main.cpp)
SET(cnd_src  ${cnd_src} ChaosDataService.cpp)

ADD_EXECUTABLE(ChaosDataService ${cnd_src})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosDataService pthread dl chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosDataService chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

INSTALL_TARGETS(/bin ChaosDataService)