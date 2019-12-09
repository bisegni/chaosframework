cmake_minimum_required(VERSION 2.8)
MESSAGE("framework CMakeLists")

# workaround for issues with MacOS 10.15 Catalina and stack_not_16_byte_aligned_error issues
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-check -fno-stack-protector")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-check -fno-stack-protector")


IF(WIN32)
add_definitions(-D_WIN32_WINNT=0x601)
#include(config/CMakeChaosWin.txt)
macro (MESG parm)
  SET(WHERE ${CMAKE_CURRENT_LIST_DIR})
  MESSAGE(STATUS "[${WHERE}] ${parm}")
endmacro()
ELSE(WIN32)
include(config/CMakeChaos.txt)
ENDIF(WIN32)
project(chaosframework)
include(ExternalProject)
#include(CTest)
#enable_testing()

#find git
find_package(Git REQUIRED)

#clear var
unset(CMAKE_CXX_COMPILER_WORKS CACHE)
unset(CMAKE_CXX_COMPILER_WORKS)
#specify option
option(USE_GPT "Uses Google Performance tool to perform analisys" OFF)
option(CLING_VIRTUAL_MACHINE_ENABLE "Enable cling for scripting" OFF)
option(CHAOS_ONLY_DEPENDECY "Download all the dependency" OFF)
option(CHAOS_ARCHITECTURE_TEST "Architecture tests" OFF)
option(CHAOS_PROMETHEUS "Enable Prometheus metrics support" OFF)
if (WIN32)
option(CHAOS_MDS "Chaos Data Service" OFF)
option(CHAOS_AGENT "Chaos Agent Service" OFF)
option(CHAOS_DATA_EXPORT "Chaos Data Export" OFF)
option(CHAOS_EXAMPLES "Chaos Examples" OFF)
#warning possibile perdita di dati spento.
add_definitions(-wd"4267")


SET (ALL_WINDOWS_LIBPATH "DbgHelp.lib Netapi32.lib Iphlpapi.lib")



else (WIN32)
option(CHAOS_MDS "Chaos Data Service" ON)
option(CHAOS_AGENT "Chaos Agent Service" ON)
option(CHAOS_DATA_EXPORT "Chaos Data Export" ON)
option(CHAOS_EXAMPLES "Chaos Examples" ON)
endif(WIN32)
option(CHAOS_CCS "Chaos CCS Compile needs qt5.6-static-x86_64 installed" OFF)
if(NOT QMAKE_PATH)
  set(QMAKE_PATH /usr/local/chaos/qt-56/bin)
ENDIF()

EXECUTE_PROCESS(
COMMAND sh ./version.sh
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
if (NOT WIN32)
CheckConfigureBuild(jsoncpp jsoncpp "-DJSONCPP_WITH_TESTS=OFF;-DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF" https://github.com/bisegni )
if(CHAOS_MDS)
  CheckConfigureBuild(mongoclient mongo-cxx-driver:legacy-1.0.0-rc0 "--extralib=boost_system;--disable-warnings-as-errors;install-mongoclient" https://github.com/mongodb)
ENDIF()
endif()
#########

#IF(CHAOS_ARCHITECTURE_TEST)
#    SET (CMAKE_BUILD_TYPE Debug)
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage")
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage")
#    set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
#ENDIF()

IF (CHAOS_ONLY_DEPENDECY OR
    CHAOS_ARCHITECTURE_TEST)
  #google test
   MESG("Add google Test as external project")
  IF(WIN32)
   ExternalProject_Add(
     googletest
     GIT_REPOSITORY https://github.com/google/googletest.git
	 GIT_TAG release-1.10.0
     PREFIX "${CMAKE_BINARY_DIR}/ext_dep/googletest-prefix"
     SOURCE_DIR "${CMAKE_BINARY_DIR}/ext_dep/googletest-src"
     BINARY_DIR "${CMAKE_BINARY_DIR}/ext_dep/googletest-build"
     CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}  -DCMAKE_BUILD_TYPE=Release
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
  ELSE(WIN32)
 
  ExternalProject_Add(
     googletest
     GIT_REPOSITORY https://github.com/google/googletest.git
	 GIT_TAG release-1.8.0
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

   ENDIF(WIN32)
ENDIF()

IF (USE_GPT)
    #google performance tools
    MESG("Add Google Performance Tools dependency")
    ExternalProject_Add(
    google_pt
    GIT_REPOSITORY https://github.com/gperftools/gperftools.git
    GIT_TAG master
    PREFIX "${CMAKE_BINARY_DIR}/ext_dep/gpt-prefix"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/ext_dep/gpt-src"
    BINARY_DIR "${CMAKE_BINARY_DIR}/ext_dep/gpt-src"
    CONFIGURE_COMMAND ${CMAKE_BINARY_DIR}/ext_dep/gpt-src/./autogen.sh COMMAND ${CMAKE_BINARY_DIR}/ext_dep/gpt-src/./configure --prefix=${CMAKE_INSTALL_PREFIX}
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

IF (CHAOS_PROMETHEUS)
  #google performance tools
  MESG("Add Prometheus metrics dependency")
  ExternalProject_Add(
  prometheus
  GIT_REPOSITORY https://github.com/jupp0r/prometheus-cpp.git
  GIT_TAG v0.8.0
  PREFIX "${CMAKE_BINARY_DIR}/ext_dep/prometheus-prefix"
  SOURCE_DIR "${CMAKE_BINARY_DIR}/ext_dep/prometheus-src"
  BINARY_DIR "${CMAKE_BINARY_DIR}/ext_dep/prometheus-src"
  CMAKE_ARGS  -DBUILD_SHARED_LIBS=ON
                -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
  LOG_DOWNLOAD ON
  LOG_CONFIGURE ON
  LOG_BUILD ON)
  # add definition for compilation
  add_definitions(-DCHAOS_PROMETHEUS=1)
ENDIF()

SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )
# ADD_DEFINITIONS(-Wno-ignored-attributes)
INCLUDE_DIRECTORIES(. ../ ../../ ${CMAKE_INSTALL_PREFIX}/include)
LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/lib)
LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/lib64)
LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/lib/x86_64-linux-gnu)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

ADD_DEFINITIONS(${GCC_COVERAGE_COMPILE_FLAGS})
MESSAGE(STATUS "COMMON COMPILE FLAGS:${GCC_COVERAGE_COMPILE_FLAGS}")

if(NOT CHAOS_ONLY_DEPENDECY)
  MESG("Configure Common Layer")
  ADD_SUBDIRECTORY(chaos/common)

  MESG("Configure CUToolkit Layer")
  ADD_SUBDIRECTORY(chaos/cu_toolkit)

  MESG("Configure Chaos Metadata Service Client Library")
  ADD_SUBDIRECTORY(chaos_metadata_service_client)

  MESG("Configure Chaos Micro Unit Toolkit")
  ADD_SUBDIRECTORY(chaos_micro_unit_toolkit)

endif()

IF (CHAOS_AGENT AND NOT CHAOS_ONLY_DEPENDECY)
  MESG("Configure Chaos Agent Service")
  ADD_SUBDIRECTORY(ChaosAgent)
ENDIF()

IF (CHAOS_MDS AND
    NOT CHAOS_ONLY_DEPENDECY)
    # IF("${CMAKE_CXX_COMPILE_FEATURES}" MATCHES "cxx_std_11")
    MESG("Configure Chaos Data Service")
    ADD_SUBDIRECTORY(ChaosMetadataService)
    # ELSE()
    # WARN("Chaos Data Service require a c11 compliant compiler")
    # ENDIF()
ENDIF()

IF(NOT CHAOS_ONLY_DEPENDECY)
  if (NOT WIN32)
  MESG("Configure Chaos Metadata Service Client Library Test Application")
  ADD_SUBDIRECTORY(ChaosMetadataServiceClientTest)
  endif()
  IF (CHAOS_DATA_EXPORT)
    MESG("Configure Chaos Data Export ")
    ADD_SUBDIRECTORY(ChaosDataExport)
  ENDIF()

  IF(CHAOS_EXAMPLES)
    MESG("Configure Chaos ChaosMDSCmd")
    ADD_SUBDIRECTORY(example/ChaosMDSCmd)
  ENDIF()
ENDIF()

IF (CHAOS_ARCHITECTURE_TEST)
#scan all subdirectory of test directory
    MESG("Configure infrastructure tests")
    set_directory_properties(PROPERTIES GTEST_INCLUDE_DIR ${GTEST_INCLUDE_DIR}
                                        GTEST_LIBRARY_DIR ${GTEST_LIBRARY_DIR}
                                        GTEST_LIBRARY_PATH ${GTEST_LIBRARY_PATH})
    ADD_SUBDIRECTORY(TestFramework)
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
IF (NOT WIN32)
    IF( NOT ${CMAKE_INSTALL_PREFIX} MATCHES ${CMAKE_CURRENT_SOURCE_DIR}/usr/local)
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
        if(err)
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
