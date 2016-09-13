cmake_minimum_required(VERSION 2.8)
project(ChaosDataService)
SET(src main.cpp)
SET(src  ${src} ../chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.cpp)

SET(src  ${src} ChaosDataService.cpp
                DriverPoolManager.cpp
                QueryDataConsumer.cpp)

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
                db_system/MongoDBDriver.cpp)

SET(src  ${src} object_storage/mongodb/MongoDBObjectStorageDriver.cpp
                object_storage/mongodb/MongoDBObjectStorageDataAccess.cpp
                object_storage/abstraction/ObjectStorageDataAccess.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${src})

CheckConfigureBuild(couchbaseS libcouchbase:chaos-0.1 "-DLCB_NO_SSL=true;-DLCB_NO_TESTS=true;-DLCB_NO_TOOLS=true;-DLCB_NO_PLUGINS=true" https://github.com/amichelotti)

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} mongoclient couchbaseS chaos_common ${FrameworkLib})
INSTALL_TARGETS(/bin ${PROJECT_NAME})
