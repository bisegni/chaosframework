cmake_minimum_required(VERSION 2.8)
include(config/CMakeChaos.txt)
project(chaosframework)

include(CTest)

option(CHAOS_MEMCACHE_TEST "Memcached test" OFF)
option(CHAOS_ARCHITECTURE_TEST "Architecture tests" OFF)
option(CHAOS_CDS "Chaos Data Service" ON)
option(CHAOS_MDS "Chaos MetaData Service" ON)
option(CHAOS_WAN "Chaos Wan Service" ON)
option(CHAOS_DATA_EXPORT "Chaos Data Export" ON)
option(CHAOS_EXAMPLES "Chaos Examples" ON)
option(CHAOS_CCS "Chaos CCS Compile needs qt5.6-static-x86_64 installed" OFF)


EXECUTE_PROCESS(
COMMAND ./version.sh
WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/chaos/common/
RESULT_VARIABLE err
    OUTPUT_VARIABLE out
)
if(err)
	ERROR("CANNOT generate ${CMAKE_CURRENT_LIST_DIR}/chaos/common/version.h: ${out}")
	ELSE()
	MESG("created version.h")
ENDIF()

###### Required Libraries
CheckConfigureBuild(jsoncpp jsoncpp "-DJSONCPP_WITH_TESTS=OFF;-DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF" https://github.com/bisegni )
CheckConfigureBuild(mongoose mongoose-cpp "-DHAS_JSONCPP=ON;-DENABLE_PIC=ON" https://github.com/bisegni )
CheckConfigureBuild(mongoclient mongo-cxx-driver:legacy-1.0.0-rc0 "--extralib=boost_system;--disable-warnings-as-errors;install-mongoclient" https://github.com/mongodb)

#########


SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )





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
#scan all subdirectory of test directory
    MESG("Configure infrastructure tests")
    SET(CHAOS_TEST_DIRECTORY "CHAOSFrameworkTests")

    SUBDIRLIST(TEST_SUBDIRS ${CHAOS_TEST_DIRECTORY})
    FOREACH(_TEST_DIRECTORY ${TEST_SUBDIRS})
        IF(EXISTS "${_TEST_DIRECTORY}/CMakeLists.txt")
            get_filename_component(_TEST_DIR_NAME ${_TEST_DIRECTORY} NAME)
            MESG("Configure ${_TEST_DIR_NAME} test")
            ADD_SUBDIRECTORY(${_TEST_DIRECTORY})
        ENDIF()
    ENDFOREACH()
ENDIF()

MESG("Done")

#MESG("Configure Plugin Test")
#ADD_SUBDIRECTORY(test/PluginTest bin/plugin_test)
if( NOT ${CMAKE_INSTALL_PREFIX} MATCHES ${CMAKE_CURRENT_SOURCE_DIR}/usr/local)
  IF(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/usr/local" )
   FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/usr/)
   execute_process(
     COMMAND ln -sf ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_SOURCE_DIR}/usr/local
     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
     RESULT_VARIABLE err
     OUTPUT_VARIABLE out
     )
   IF(err)
     ERROR("cannot create symlink ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_SOURCE_DIR}/usr/local")
   ELSE()
    MESG("created symlink ${CMAKE_INSTALL_PREFIX}->${CMAKE_CURRENT_SOURCE_DIR}/usr/local")
  ENDIF()
ENDIF()
ENDIF()
  IF(CHAOS_CCS)
    MESG("CCS compilation ENABLED")
    IF(EXISTS "/usr/local/chaos/qt5.6-static-x86_64/bin/qmake")
      execute_process(
	COMMAND /usr/local/chaos/qt5.6-static-x86_64/bin/qmake
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ccs
	RESULT_VARIABLE err
	OUTPUT_VARIABLE out
	)
      if( err)
	ERROR("error generating QT stubs error: ${out}")
      endif()

      execute_process(
	COMMAND make -j ${PROCESSOR_COUNT}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ccs
	RESULT_VARIABLE err
	OUTPUT_VARIABLE out
	)
      if( err)
	ERROR("error compiling: ${out}")
      endif()
      FILE(GLOB exe ccs/ccs ccs/ccs.sh)
      FILE(COPY ${exe} DESTINATION bin/ FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
    ELSE()
      ERROR("missing /usr/local/chaos/qt5.6-static-x86_64/bin/qmake")
    ENDIF()
  ENDIF()

#add test
