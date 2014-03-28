SET(src main.cpp)
SET(src  ${src} ChaosDataService.cpp DataConsumer.cpp)
SET(src  ${src} worker/DataWorker.cpp worker/DeviceSharedDataWorker.cpp worker/AnswerDataWorker.cpp)
SET(src  ${src} cache_system/CacheDriver.cpp cache_system/MemcachedCacheDriver.cpp cache_system/CouchbaseCacheDriver.cpp)
SET(src  ${src} storage_system/StorageDriver.cpp storage_system/PosixStorageDriver.cpp)
SET(src  ${src} index_system/IndexDriver.cpp index_system/MongoDBIndexDriver.cpp)

ADD_EXECUTABLE(ChaosDataService ${src})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosDataService pthread dl couchbase chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosDataService couchbase chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

INSTALL_TARGETS(/bin ChaosDataService)