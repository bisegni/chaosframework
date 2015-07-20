cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

SET(src main.cpp)

SET(src  ${src} ../chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.cpp)

SET(src  ${src} ChaosDataService.cpp
                QueryDataConsumer.cpp
                StageDataConsumer.cpp)

SET(src  ${src} worker/DataWorker.cpp
                worker/DeviceSharedDataWorker.cpp
                worker/SnapshotCreationWorker.cpp)

SET(src  ${src} cache_system/CacheDriver.cpp
                cache_system/MemcachedCacheDriver.cpp
				cache_system/CouchbaseCacheDriver.cpp
                cache_system/CacheDriverMetricCollector.cpp
                cache_system/CacheDriverSharedMetricIO.cpp)

SET(src  ${src} db_system/DBDriver.cpp
                db_system/MongoDBDriver.cpp
				db_system/DBIndexCursor.cpp
                db_system/MongoDBIndexCursor.cpp)

SET(src  ${src} query_engine/QueryEngine.cpp)

SET(src  ${src} indexer/DataPackScanner.cpp
                indexer/StageDataVFileScanner.cpp)

SET(src  ${src} vfs/VFSManager.cpp
                vfs/VFSFile.cpp
                vfs/VFSStageFile.cpp
                vfs/VFSDataFile.cpp
				vfs/VFSDataWriteableFile.cpp
                vfs/VFSStageWriteableFile.cpp
                vfs/VFSStageReadableFile.cpp
				vfs/storage_system/StorageDriver.cpp
                vfs/storage_system/PosixStorageDriver.cpp
				vfs/VFSQuery.cpp)

SET(src  ${src} vfs/query/DataBlockCache.cpp
                vfs/query/DataBlockFetcher.cpp)

ADD_EXECUTABLE(ChaosDataService ${src})


IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(ChaosDataService mongoclient couchbase chaos_common ${FrameworkLib})
INSTALL_TARGETS(/bin ChaosDataService)
