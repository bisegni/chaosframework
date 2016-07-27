cmake_minimum_required(VERSION 2.6)
project (chaos)
include(config/CMakeChaos.txt)
option(CHAOS_MEMCACHE_TEST "Memcached test" OFF)
option(CHAOS_ARCHITECTURE_TEST "Architecture tests" ON)
option(CHAOS_CDS "Chaos Data Service" ON)
option(CHAOS_MDS "Chaos MetaData Service" ON)
option(CHAOS_WAN "Chaos Wan Service" ON)
option(CHAOS_DATA_EXPORT "Chaos Data Export" ON)
option(CHAOS_EXAMPLES "Chaos Examples" ON)



SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )




EXECUTE_PROCESS(COMMAND chaos/common/version.sh)

INCLUDE_DIRECTORIES(. ../ ../../ ${CMAKE_INSTALL_PREFIX}/include)
LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/lib)

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

MESG("Configure Common Layer")
ADD_SUBDIRECTORY(chaos/common)

MESG("Configure CUToolkit Layer")
ADD_SUBDIRECTORY(chaos/cu_toolkit)

MESG("Configure UIToolkit Layer")
ADD_SUBDIRECTORY(chaos/ui_toolkit)

IF (CHAOS_MDS)
  MESG("Configure Chaos Metadata Service")
  ADD_SUBDIRECTORY(ChaosMetadataService)
ENDIF()

MESG("Configure Chaos Metadata Service Client Library")
ADD_SUBDIRECTORY(ChaosMetadataServiceClient)

MESG("Configure Chaos Metadata Service Client Library Test Application")
ADD_SUBDIRECTORY(ChaosMetadataServiceClientTest)

IF (CHAOS_WAN)
  MESG("Configure Chaos WAN Proxy")
  ADD_SUBDIRECTORY(ChaosWANProxy)
ENDIF()

IF (CHAOS_CDS)
  MESG("Configure Chaos Data Service")
  ADD_SUBDIRECTORY(ChaosDataService)
ENDIF()

IF (CHAOS_DATA_EXPORT)
  MESG("Configure Chaos Data Export ")
  ADD_SUBDIRECTORY(ChaosDataExport)
ENDIF()

IF(CHAOS_EXAMPLES)
  MESG("Configure Chaos CLI")
  ADD_SUBDIRECTORY(example/ChaosCLI)

  MESG("Configure Chaos ChaosMDSCmd")
  ADD_SUBDIRECTORY(example/ChaosMDSCmd)

  MESG("Configure Chaos Performance Tester")
  ADD_SUBDIRECTORY(example/ChaosPerformanceTester)
#MESG("Configure Control Unit Test examples")
#ADD_SUBDIRECTORY(example/ControlUnitTest bin/bin_example_cutest)

#MESG("Configure BenchTest simulation excutable")
#ADD_SUBDIRECTORY(example/BenchTest bin/bench_test)

ENDIF()

IF (CHAOS_MEMCACHE_TEST)
  MESG("Configure ChaosMemCacheTest test excutable")
  ADD_SUBDIRECTORY(test/MemcachedEmbeddedTest)
ENDIF()

IF (CHAOS_ARCHITECTURE_TEST)
  MESG("Configure ChaosArchitectureTests test excutable")
  ADD_SUBDIRECTORY(ArchitectureTests)
ENDIF()

MESG("Done")
# MESG("Configure Chaos Snapshot Utility")
# ADD_SUBDIRECTORY(ChaosSnapshotUtility/ChaosSnapshotUtility bin/chaos_snap_util)

#MESG("Configure Plugin Test")
#ADD_SUBDIRECTORY(test/PluginTest bin/plugin_test)
