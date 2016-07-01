cmake_minimum_required(VERSION 2.6)
IF (DEFINED ENV{CHAOS_BUNDLE} )
  include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
ELSE()
  SET(FrameworkLib boost_program_options boost_date_time boost_system  boost_chrono boost_regex boost_log_setup boost_log boost_filesystem boost_thread boost_atomic memcached zmq jsoncpp pthread rt dl)
ENDIF()

project (chaos)

SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )


add_definitions(-DDEBUG)


EXECUTE_PROCESS(COMMAND chaos/common/version.sh)

ADD_DEFINITIONS(-O3 )
INCLUDE_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/include)
LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/lib)
IF(NOT BUILD_PREFIX)
set (BUILD_PREFIX usr/local)
ENDIF()

message(STATUS "Using ${BUILD_PREFIX} as installation directory")

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

message(STATUS "Configure Common Layer")
ADD_SUBDIRECTORY(chaos/common bin/bin_common)

message(STATUS "Configure CUToolkit Layer")
ADD_SUBDIRECTORY(chaos/cu_toolkit bin/bin_cutoolkit)

message(STATUS "Configure UIToolkit Layer")
ADD_SUBDIRECTORY(chaos/ui_toolkit bin/bin_uitoolkit)

IF (NOT CHAOS_NO_MONGO)
message(STATUS "Configure Chaos Metadata Service")
ADD_SUBDIRECTORY(ChaosMetadataService bin/bin_metadata_service)
ENDIF()

message(STATUS "Configure Chaos Metadata Service Client Library")
ADD_SUBDIRECTORY(ChaosMetadataServiceClient bin/bin_metadata_service_client)

message(STATUS "Configure Chaos Metadata Service Client Library Test Application")
ADD_SUBDIRECTORY(ChaosMetadataServiceClientTest bin/ChaosMetadataServiceClientTest)

message(STATUS "Configure Chaos WAN Proxy")
ADD_SUBDIRECTORY(ChaosWANProxy bin/bin_wan_proxy)

IF (NOT CHAOS_NO_MONGO)
message(STATUS "Configure Chaos Data Service")
ADD_SUBDIRECTORY(ChaosDataService bin/bin_cds)
ENDIF()

message(STATUS "Configure Chaos Data Export")
ADD_SUBDIRECTORY(ChaosDataExport bin/chaos_cde)

message(STATUS "Configure Chaos CLI")
ADD_SUBDIRECTORY(example/ChaosCLI bin/chaos_cli)

message(STATUS "Configure Chaos ChaosMDSCmd")
ADD_SUBDIRECTORY(example/ChaosMDSCmd bin/ChaosMDSCmd)

message(STATUS "Configure Chaos Performance Tester")
ADD_SUBDIRECTORY(example/ChaosPerformanceTester bin/ChaosPerformanceTester)

#message(STATUS "Configure Control Unit Test examples")
#ADD_SUBDIRECTORY(example/ControlUnitTest bin/bin_example_cutest)

#message(STATUS "Configure BenchTest simulation excutable")
#ADD_SUBDIRECTORY(example/BenchTest bin/bench_test)

message(STATUS "Configure ChaosMemCacheTest test excutable")
ADD_SUBDIRECTORY(test/MemcachedEmbeddedTest bin/cache_test)

message(STATUS "Configure ChaosArchitectureTests test excutable")
ADD_SUBDIRECTORY(ArchitectureTests bin/architecture_test)

# message(STATUS "Configure Chaos Snapshot Utility")
# ADD_SUBDIRECTORY(ChaosSnapshotUtility/ChaosSnapshotUtility bin/chaos_snap_util)

#message(STATUS "Configure Plugin Test")
#ADD_SUBDIRECTORY(test/PluginTest bin/plugin_test)
