cmake_minimum_required(VERSION 2.8)

include(config/CMakeChaos.txt)
project(chaosframework)
include(ExternalProject)
#include(CTest)
#find git
find_package(Git REQUIRED)

#clear var
unset(CMAKE_CXX_COMPILER_WORKS CACHE)
unset(CMAKE_CXX_COMPILER_WORKS)

#specify option
option(CHAOS_ONLY_DEPENDECY "Download all the dependency" OFF)
option(CHAOS_ARCHITECTURE_TEST "Architecture tests" OFF)
option(USE_GPT "Uses Google Performance tool to perform analisys" OFF)
option(CLING_VIRTUAL_MACHINE "Enable cling for scripting" OFF)
option(CHAOS_CDS "Chaos Data Service" ON)
option(CHAOS_MDS "Chaos MetaData Service" ON)
option(CHAOS_AGENT "Chaos Agent Service" ON)
option(CHAOS_DATA_EXPORT "Chaos Data Export" ON)
option(CHAOS_EXAMPLES "Chaos Examples" ON)
option(CHAOS_CCS "Chaos CCS Compile needs qt5.6-static-x86_64 installed" OFF)
set(QMAKE_PATH /usr/local/chaos/qt-56/bin)

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

CheckConfigureBuild(mongoclient mongo-cxx-driver:legacy-1.0.0-rc0 "--extralib=boost_system;--disable-warnings-as-errors;install-mongoclient" https://github.com/mongodb)
#########


IF (CHAOS_ONLY_DEPENDECY OR
    CHAOS_ARCHITECTURE_TEST)
  #google test
  MESG("Add Google Test as external project")
  ExternalProject_Add(
     googletest
     GIT_REPOSITORY https://github.com/google/googletest.git
     GIT_TAG master
     PREFIX "${CMAKE_BINARY_DIR}/ext_dep/googletest-prefix"
     SOURCE_DIR "${CMAKE_BINARY_DIR}/ext_dep/googletest-src"
     BINARY_DIR "${CMAKE_BINARY_DIR}/ext_dep/googletest-build"
     CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
     LOG_DOWNLOAD ON
     LOG_CONFIGURE ON
     LOG_BUILD ON)

     # Specify include dir
     ExternalProject_Get_Property(googletest source_dir)
     set(GTEST_INCLUDE_DIR ${source_dir}/include)

     # Library
     ExternalProject_Get_Property(googletest binary_dir)
     set(GTEST_LIBRARY_DIR ${binary_dir})
     ExternalProject_Get_Property(googletest binary_dir)
     set(GTEST_LIBRARY_PATH ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a)

     MESG("${GTEST_INCLUDE_DIR}")
     MESG("${GTEST_LIBRARY_DIR}")
     MESG("${GTEST_LIBRARY_PATH}")
     MESG("${GTEST_LIBRARY}")
ENDIF()

IF (USE_GPT)
    #google performance tools
    MESG("Add Goolge Performance Tools as external project")
    ExternalProject_Add(
    google_pt
    GIT_REPOSITORY https://github.com/gperftools/gperftools.git
    GIT_TAG master
    PREFIX "${CMAKE_BINARY_DIR}/ext_dep/gpt-prefix"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/ext_dep/gpt-src"
    BINARY_DIR "${CMAKE_BINARY_DIR}/ext_dep/gpt-src"
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/ext_dep/gpt-src/./autogen.sh COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/ext_dep/gpt-src/./configure --prefix=${CMAKE_INSTALL_PREFIX}
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)

    # Specify include dir
    ExternalProject_Get_Property(google_pt source_dir)
    set(GPT_INCLUDE_DIR ${google_pt_sdir}/include)

    # Library
    ExternalProject_Get_Property(google_pt binary_dir)
    set(GPT_LIBRARY_DIR ${google_pt_bdir})

    MESG("${GPT_INCLUDE_DIR}")
    MESG("${GPT_LIBRARY_DIR}")
ENDIF()

IF (CLING_VIRTUAL_MACHINE)
  IF(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/ext_dep/cling-src)
    MESG("Create directory for cling download files")
    EXECUTE_PROCESS(
      COMMAND mkdir -p ${CMAKE_CURRENT_LIST_DIR}/ext_dep/cling-src;
      RESULT_VARIABLE err
      OUTPUT_VARIABLE out
    )

    MESG("Downloading cling")
    EXECUTE_PROCESS(
    COMMAND ${CMAKE_CURRENT_LIST_DIR}/config/cling_clone.sh ${CMAKE_CURRENT_LIST_DIR}/ext_dep/cling-src;
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/ext_dep/cling-src
    RESULT_VARIABLE err
        OUTPUT_VARIABLE out
    )
  ENDIF()

  IF(err)
  	ERROR("CANNOT create ${CMAKE_CURRENT_LIST_DIR}/ext_dep/cling-src directory: ${out}")
  ELSE()
    #cling
    MESG("Add cling as external project")
    ExternalProject_Add(
       cling
       PREFIX "${CMAKE_BINARY_DIR}/ext_dep/cling-prefix"
       SOURCE_DIR "${CMAKE_BINARY_DIR}/ext_dep/cling-src"
       BINARY_DIR "${CMAKE_BINARY_DIR}/ext_dep/cling-build"
       CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
       LOG_DOWNLOAD ON
       LOG_CONFIGURE ON
       LOG_BUILD ON)
  ENDIF()
ENDIF()

SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )
# ADD_DEFINITIONS(-Wno-ignored-attributes)
INCLUDE_DIRECTORIES(. ../ ../../ ${CMAKE_INSTALL_PREFIX}/include)
LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/lib)

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

if(NOT CHAOS_ONLY_DEPENDECY)
  MESG("Configure Common Layer")
  ADD_SUBDIRECTORY(chaos/common)
  GET_DIRECTORY_PROPERTY(CLING_VIRTUAL_MACHINE DIRECTORY chaos/common VARIABLES CLING_VIRTUAL_MACHINE)

  MESG("Configure CUToolkit Layer")
  ADD_SUBDIRECTORY(chaos/cu_toolkit)

  MESG("Configure UIToolkit Layer")
  ADD_SUBDIRECTORY(chaos/ui_toolkit)

  MESG("Configure Chaos Metadata Service Client Library")
  ADD_SUBDIRECTORY(chaos_metadata_service_client)

  MESG("Configure Chaos Micro Unit Toolkit")
  ADD_SUBDIRECTORY(chaos_micro_unit_toolkit)
endif()

IF (CHAOS_AGENT AND NOT CHAOS_ONLY_DEPENDECY)
MESG("Configure Chaos Agent Service")
ADD_SUBDIRECTORY(ChaosAgent)
ENDIF()

IF (CHAOS_MDS AND NOT CHAOS_ONLY_DEPENDECY)
  MESG("Configure Chaos Metadata Service")
  ADD_SUBDIRECTORY(ChaosMetadataService)
ENDIF()

IF(NOT CHAOS_ONLY_DEPENDECY)
  MESG("Configure Chaos Metadata Service Client Library Test Application")
  ADD_SUBDIRECTORY(ChaosMetadataServiceClientTest)


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
  ENDIF()
ENDIF()

IF (CHAOS_ARCHITECTURE_TEST)
#scan all subdirectory of test directory
    MESG("Configure infrastructure tests")
    set_directory_properties(PROPERTIES GTEST_INCLUDE_DIR ${GTEST_INCLUDE_DIR}
                                        GTEST_LIBRARY_DIR ${GTEST_LIBRARY_DIR}
                                        GTEST_LIBRARY_PATH ${GTEST_LIBRARY_PATH})
    ADD_SUBDIRECTORY(CHAOSFrameworkTests/FrameworkTest)
#    SET(CHAOS_TEST_DIRECTORY "CHAOSFrameworkTests")
#    SUBDIRLIST(TEST_SUBDIRS ${CHAOS_TEST_DIRECTORY})
#    FOREACH(_TEST_DIRECTORY ${TEST_SUBDIRS})
#        IF(EXISTS "${_TEST_DIRECTORY}/CMakeLists.txt")
#            #set the external project variable in subdirectory
#            get_filename_component(_TEST_DIR_NAME ${_TEST_DIRECTORY} NAME)
#            MESG("Configure ${_TEST_DIR_NAME} test")
#            ADD_SUBDIRECTORY(${_TEST_DIRECTORY})
#        ENDIF()
#    ENDFOREACH()
ENDIF()

MESG("Done")

if( NOT ${CMAKE_INSTALL_PREFIX} MATCHES ${CMAKE_CURRENT_SOURCE_DIR}/usr/local)
  IF(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/usr/local" )
    FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/usr)
    MESG("linking ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_SOURCE_DIR}/usr/local")
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
    MESG("CCS compilation ENABLED.")
    IF(EXISTS "${QMAKE_PATH}/qmake")
      MESG("Found 'qmake' in ${QMAKE_PATH}")

      execute_process(
	COMMAND ${QMAKE_PATH}/qmake
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ccs
	RESULT_VARIABLE err
	OUTPUT_VARIABLE out
	)
      if( err)
	ERROR("error generating QT stubs error: ${out}")
      endif()

       add_custom_target(ccs
       	COMMAND make -j ${PROCESSOR_COUNT}
       	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ccs
       	DEPENDS chaos_common
       	COMMENT "compiling CCS"
       	)


    ELSE()
      ERROR("missing ${QMAKE_PATH}/qmake")
    ENDIF()

  ENDIF()

  IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ccs/ccs)
    MESG("copying ccs to ${CMAKE_INSTALL_PREFIX}/bin")
    FILE(GLOB exe ${CMAKE_CURRENT_SOURCE_DIR}/ccs/ccs.sh ${CMAKE_CURRENT_SOURCE_DIR}/ccs/ccs)
    FILE(COPY ${exe} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/ FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
  ENDIF()
INSTALL(DIRECTORY tools DESTINATION ${CMAKE_INSTALL_PREFIX} PATTERN "*" PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ)

#add test
