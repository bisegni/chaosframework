cmake_minimum_required(VERSION 2.6)
option(CHAOS_MEMCACHE_TEST "Memcached test" OFF)
option(CHAOS_ARCHITECTURE_TEST "Architecture tests" ON)

SET(GITSOURCE https://github.com/amichelotti)

macro (GitClone arg)
  message(STATUS "[${arg}] Cloning into ${CMAKE_CURRENT_SOURCE_DIR}/external")
  file(REMOVE_RECURSE  ${CMAKE_CURRENT_SOURCE_DIR}/external)
  file(MAKE_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/external)
  execute_process(
    COMMAND git clone ${GITSOURCE}/${arg}.git
    WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/external
    RESULT_VARIABLE err
    OUTPUT_VARIABLE mod
    )
  if( err)
    message(FATAL_ERROR "[${arg}] cloning  error: ${err}")
  endif()
endmacro()

macro (GitCheckout arg arg1)
  message(STATUS "[${arg}] Checkout branch ${arg1}")

  execute_process(
    COMMAND git checkout ${arg1}
    WORKING_DIRECTORY  ${CMAKE_CURRENT_SOURCE_DIR}/external/${arg}
    RESULT_VARIABLE err
    OUTPUT_VARIABLE out)
  if( err)
    message(FATAL_ERROR "[${arg}] checkout  error: ${err}")
  endif()
endmacro()

macro (ConfigureAndBuild arg arg1)
  IF (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/${arg}/CMakeLists.txt")
    SET(CONFPARM cmake ${arg1} ${CMAKE_CHAOS} .)
  ELSE()
    IF (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/${arg}/autogen.sh")
      message(STATUS "[${arg}] generation of configuration")
      execute_process(
	COMMAND ./autogen.sh
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/${arg}
	RESULT_VARIABLE err
	OUTPUT_VARIABLE mod
	)

      IF( err)
	message(FATAL_ERROR "[${arg}] performing generation of configuration: ${err}")
      ENDIF()
    ENDIF()
    SET(CONFPARM ./configure --prefix=${CMAKE_INSTALL_PREFIX} ${arg1} ${CROSS_HOST})
  ENDIF()
   #  SET(CONFPARM ./configure ${CROSS_HOST} ${arg1} --prefix=${CMAKE_INSTALL_PREFIX})

   separate_arguments(CONFPARM)
   message(STATUS "[${arg}] configuring with \"${CONFPARM}\"")

  execute_process(
    COMMAND ${CONFPARM}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/${arg}
    RESULT_VARIABLE err
    OUTPUT_VARIABLE out
    )

  if( err)
    message(FATAL_ERROR "[${arg}] configuring ${arg} :${err}")
  endif()

  message(STATUS "[${arg}] compiling (${PROCESSOR_COUNT})")
  execute_process(
    COMMAND make install -j ${PROCESSOR_COUNT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/${arg}
    RESULT_VARIABLE err2
    OUTPUT_VARIABLE out2
    )

  if( err2)
    message(FATAL_ERROR "[${arg}] compiling ${out2}: ${err2}")
  endif()


endmacro()


# IF (DEFINED ENV{CHAOS_BUNDLE} )
#   include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
# ELSE()

EXECUTE_PROCESS(COMMAND uname -s OUTPUT_VARIABLE SYSTEM_NAME)
MESSAGE(STATUS " System : ${SYSTEM_NAME}")
IF(${SYSTEM_NAME} MATCHES "Darwin")
   # Mac OS X specific code
   MESSAGE(STATUS " enabling MAC compilation")
  SET(FrameworkLib boost_program_options boost_date_time boost_system  boost_chrono boost_regex boost_log_setup boost_log boost_filesystem boost_thread boost_atomic zmq.a jsoncpp pthread dl)
ELSE()
  SET(FrameworkLib boost_program_options boost_date_time boost_system  boost_chrono boost_regex boost_log_setup boost_log boost_filesystem boost_thread boost_atomic zmq.a jsoncpp pthread rt dl)
ENDIF()
project (chaos)

SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )


add_definitions(-DDEBUG )
SET(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++")

EXECUTE_PROCESS(COMMAND chaos/common/version.sh)

ADD_DEFINITIONS(-O3 )
INCLUDE_DIRECTORIES(. ../ ../../ ${CMAKE_INSTALL_PREFIX}/include)
LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/lib)
IF(NOT BUILD_PREFIX)
set (BUILD_PREFIX usr/local)
ENDIF()

message(STATUS "Using ${BUILD_PREFIX} as installation directory")

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

message(STATUS "Configure Common Layer")
ADD_SUBDIRECTORY(chaos/common)

message(STATUS "Configure CUToolkit Layer")
ADD_SUBDIRECTORY(chaos/cu_toolkit)

message(STATUS "Configure UIToolkit Layer")
ADD_SUBDIRECTORY(chaos/ui_toolkit)

IF (NOT CHAOS_NO_MONGO)
message(STATUS "Configure Chaos Metadata Service")
ADD_SUBDIRECTORY(ChaosMetadataService)
ENDIF()

message(STATUS "Configure Chaos Metadata Service Client Library")
ADD_SUBDIRECTORY(ChaosMetadataServiceClient)

message(STATUS "Configure Chaos Metadata Service Client Library Test Application")
ADD_SUBDIRECTORY(ChaosMetadataServiceClientTest)

message(STATUS "Configure Chaos WAN Proxy")
ADD_SUBDIRECTORY(ChaosWANProxy)

IF (NOT CHAOS_NO_MONGO)
message(STATUS "Configure Chaos Data Service")
ADD_SUBDIRECTORY(ChaosDataService)
ENDIF()

message(STATUS "Configure Chaos Data Export")
ADD_SUBDIRECTORY(ChaosDataExport)

message(STATUS "Configure Chaos CLI")
ADD_SUBDIRECTORY(example/ChaosCLI)

message(STATUS "Configure Chaos ChaosMDSCmd")
ADD_SUBDIRECTORY(example/ChaosMDSCmd)

message(STATUS "Configure Chaos Performance Tester")
ADD_SUBDIRECTORY(example/ChaosPerformanceTester)

#message(STATUS "Configure Control Unit Test examples")
#ADD_SUBDIRECTORY(example/ControlUnitTest bin/bin_example_cutest)

#message(STATUS "Configure BenchTest simulation excutable")
#ADD_SUBDIRECTORY(example/BenchTest bin/bench_test)
IF (CHAOS_MEMCACHE_TEST)
  message(STATUS "Configure ChaosMemCacheTest test excutable")
  ADD_SUBDIRECTORY(test/MemcachedEmbeddedTest)
ENDIF()

IF (CHAOS_ARCHITECTURE_TEST)
  message(STATUS "Configure ChaosArchitectureTests test excutable")
  ADD_SUBDIRECTORY(ArchitectureTests)
ENDIF()

# message(STATUS "Configure Chaos Snapshot Utility")
# ADD_SUBDIRECTORY(ChaosSnapshotUtility/ChaosSnapshotUtility bin/chaos_snap_util)

#message(STATUS "Configure Plugin Test")
#ADD_SUBDIRECTORY(test/PluginTest bin/plugin_test)
