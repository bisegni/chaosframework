cmake_minimum_required (VERSION 2.6)
project (chaos)

SET( ${PROJECT_NAME}_MAJOR_VERSION 0 ) 
SET( ${PROJECT_NAME}_MINOR_VERSION 1 ) 
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )

IF( NOT BUILD_FORCE_STATIC)
  add_definitions(-DBOOST_ALL_DYN_LINK)
ENDIF()

add_definitions(-DDEBUG)

OPTION( BUILD_FORCE_32 "Force 32 bit compilation" OFF )

EXECUTE_PROCESS(COMMAND chaos/common/version.sh)

INCLUDE_DIRECTORIES(. ${CMAKE_INSTALL_PREFIX}/include /usr/local/include) 
LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/lib /usr/local/lib)
ADD_DEFINITIONS(-O3 )
    
IF( BUILD_FORCE_32 ) 
    MESSAGE( STATUS "Forcing 32 bit compilation" )
    set (CMAKE_C_FLAGS "-m32 ")
    set (CMAKE_CXX_FLAGS "-m32")
    set (CMAKE_LINK_FLAGS "-m32")
ENDIF()

IF(NOT BUILD_PREFIX)
set (BUILD_PREFIX usr/local)
ENDIF()

message(STATUS "Using ${BUILD_PREFIX} as installation directory")


message(STATUS "Configure Common Layer")
ADD_SUBDIRECTORY(chaos/common bin/bin_common)

message(STATUS "Configure CUToolkit Layer")
ADD_SUBDIRECTORY(chaos/cu_toolkit bin/bin_cutoolkit)

message(STATUS "Configure UIToolkit Layer")
ADD_SUBDIRECTORY(chaos/ui_toolkit bin/bin_uitoolkit)

message(STATUS "Configure Chaos Node Directory Service")
ADD_SUBDIRECTORY(ChaosNodeDirectory bin/bin_cnd)

message(STATUS "Configure Chaos Data Service")
ADD_SUBDIRECTORY(ChaosDataService bin/bin_cds)

message(STATUS "Configure Chaos Data Export")
ADD_SUBDIRECTORY(ChaosDataExport bin/chaos_cde)


message(STATUS "Configure Chaos CLI")
ADD_SUBDIRECTORY(example/ChaosCLI bin/chaos_cli)

message(STATUS "Configure Chaos Performance Tester")
ADD_SUBDIRECTORY(example/ChaosPerformanceTester bin/ChaosPerformanceTester)

#message(STATUS "Configure Control Unit Test examples")
#ADD_SUBDIRECTORY(example/ControlUnitTest bin/bin_example_cutest)

#message(STATUS "Configure BenchTest simulation excutable")
#ADD_SUBDIRECTORY(example/BenchTest bin/bench_test)

message(STATUS "Configure ChaosMemCacheTest test excutable")
ADD_SUBDIRECTORY(test/MemcachedEmbeddedTest bin/cache_test)

message(STATUS "Configure Chaos Snapshot Utility")
ADD_SUBDIRECTORY(ChaosSnapshotUtility bin/chaos_snap_util)

#message(STATUS "Configure Plugin Test")
#ADD_SUBDIRECTORY(test/PluginTest bin/plugin_test)
