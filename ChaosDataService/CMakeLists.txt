SET(src main.cpp)
SET(src  ${src} ChaosDataService.cpp DataConsumer.cpp)
SET(src  ${src} worker/DataWorker.cpp worker/DeviceSharedDataWorker.cpp worker/AnswerDataWorker.cpp)
SET(src  ${src} cache_system/CacheDriver.cpp cache_system/MemcachedCacheDriver.cpp)

ADD_EXECUTABLE(ChaosDataService ${src})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosDataService pthread dl chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosDataService chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

INSTALL_TARGETS(/bin ChaosDataService)