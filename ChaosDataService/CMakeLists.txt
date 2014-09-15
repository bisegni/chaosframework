SET(src main.cpp)
SET(src  ${src} ChaosDataService.cpp QueryDataConsumer.cpp StageDataConsumer.cpp)

SET(src  ${src} worker/DataWorker.cpp worker/DeviceSharedDataWorker.cpp worker/AnswerDataWorker.cpp)

SET(src  ${src} cache_system/CacheDriver.cpp cache_system/MemcachedCacheDriver.cpp 
				cache_system/CouchbaseCacheDriver.cpp)

SET(src  ${src} db_system/DBDriver.cpp db_system/MongoDBDriver.cpp db_system/MongoDBHAConnectionManager.cpp
				db_system/DBIndexCursor.cpp db_system/MongoDBIndexCursor.cpp)

SET(src  ${src} query_engine/QueryEngine.cpp)

SET(src  ${src} indexer/DataPackScanner.cpp indexer/StageDataVFileScanner.cpp)

SET(src  ${src} vfs/VFSManager.cpp vfs/VFSFile.cpp vfs/VFSStageFile.cpp vfs/VFSDataFile.cpp 
				vfs/VFSDataWriteableFile.cpp
				vfs/VFSStageWriteableFile.cpp vfs/VFSStageReadableFile.cpp 
				vfs/storage_system/StorageDriver.cpp vfs/storage_system/PosixStorageDriver.cpp
				vfs/VFSQuery.cpp)

SET(src  ${src} vfs/query/DataBlockFetcher.cpp)

ADD_EXECUTABLE(ChaosDataService ${src})

SET(ChaosDataServiceLib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(ChaosDataServiceLib)
SET(ChaosDataServiceLib dl mongoclient couchbase chaos_common boost_filesystem ${ChaosDataServiceLib})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosDataService pthread ${ChaosDataServiceLib})
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosDataService ${ChaosDataServiceLib})
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

INSTALL_TARGETS(/bin ChaosDataService)
