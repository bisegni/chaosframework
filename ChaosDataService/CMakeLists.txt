cmake_minimum_required(VERSION 2.6)
project(ChaosDataService)

SET(src main.cpp)

SET(src  ${src} ../chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.cpp)

SET(src  ${src} ChaosDataService.cpp
                DriverPoolManager.cpp
                QueryDataConsumer.cpp
                StageDataConsumer.cpp)

SET(src  ${src} worker/DataWorker.cpp
                worker/DeviceSharedDataWorker.cpp
                worker/DeviceSharedDataWorkerMetric.cpp
                worker/DeviceSharedDataWorkerMetricCollector.cpp
                worker/SnapshotCreationWorker.cpp)

SET(src  ${src} cache_system/CacheDriver.cpp
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
				vfs/VFSQuery.cpp
                vfs/storage_system/StorageDriverMetricIO.cpp
                vfs/storage_system/StorageDriverMetricCollector.cpp
                vfs/storage_system/StorageDriver.cpp
                vfs/storage_system/PosixStorageDriver.cpp)

SET(src  ${src} vfs/query/DataBlockCache.cpp
                vfs/query/DataBlockFetcher.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${src})
unset(LIB_NEED CACHE)
find_library(LIB_NEED couchbaseS)

if(NOT LIB_NEED)
  MESG("library not found downloading...")
  GitClone(libcouchbase)
  GitCheckout(libcouchbase chaos-0.1)
  ConfigureAndBuild(libcouchbase  "-DLCB_NO_SSL=true;-DLCB_NO_TESTS=true;-DLCB_NO_TOOLS=true;-DLCB_NO_PLUGINS=true")
endif()


IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} mongoclient couchbaseS chaos_common ${FrameworkLib})
INSTALL_TARGETS(/bin ${PROJECT_NAME})
