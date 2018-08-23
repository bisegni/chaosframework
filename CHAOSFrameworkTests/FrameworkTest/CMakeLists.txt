cmake_minimum_required(VERSION 2.8)
project(TestFramework)
#find_package(GTest REQUIRED)
IF( CHAOS_SANITIZER MATCHES ${PROJECT_NAME} )
  MESG("ENABLING SANITIZER FOR PROJECT ${PROJECT_NAME}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=address")
  # IF( CMAKE_CXX_COMPILER_ID NOT MATCHES "Clang" )
  #   MESG("Link asan SANITIZER for non llvm compiler")
  #   SET(LinkedLibrary ${LinkedLibrary} asan)
  # ENDFI()
ENDIF()

#example plugin
SET(plugin_src plugin/ExamplePlugin.cpp )
ADD_LIBRARY(ExamplePlugin SHARED ${plugin_src})
set_target_properties(ExamplePlugin PROPERTIES PREFIX "" SUFFIX ".chaos_extension")

TARGET_LINK_LIBRARIES(ExamplePlugin chaos_cutoolkit chaos_common ${PluginTestLib})

INSTALL_TARGETS(/lib ExamplePlugin)


#tests EXECUTABLE
SET(TestFrameworkSource main.cpp
                        plugin/TestPlugin.cpp
                        script/ScriptTest.cpp
                        data/TestVariant.cpp
                        data/TestCDataWrapper.cpp
                        external_unit/ExternalUnitTest.cpp
                        external_unit/ExternalUnitMulticlientTest.cpp
                        batch_system/TestCommandExecutor.cpp
                        batch_system/TestBatchCommand.cpp
                        batch_system/TestBatchDefault.cpp
                        batch_system/BatchComandTest.cpp
                        queue/QueueTest.cpp
                        queue/QueuePriorityTest.cpp
                        property/PropertyTest.cpp
                        url_feeder/TestURLFeeder.cpp
                        state_flag_catalog/TestStateFlagCatalog.cpp
                        templated_key_value_hash/TemplateKeyValueHashMap.cpp
                        trigger/TriggerTest.cpp
                        utility/FutureHelperTest.cpp
                        direct_io/DirectIOTest.cpp
                        direct_io/DirectIODeviceServerChannelTest.cpp
                        direct_io/DirectIOSystemServerChannelTest.cpp
                        utility/FutureHelperTest.cpp)

SET(TestFrameworkSource ${TestFrameworkSource} script/ScriptTest.cpp)

IF (CLING_VIRTUAL_MACHINE_ENABLE)
  SET(TestFrameworkSource ${TestFrameworkSource} script/ScriptClingTest.cpp)
ENDIF()

INCLUDE_DIRECTORIES(. ../../usr/local/include)
LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${PROJECT_SOURCE_DIR}/../../usr/local/lib )
if(UNIX AND NOT APPLE)
      LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../usr/local/lib64)
      LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../usr/local/lib/x86_64-linux-gnu)
endif()
ADD_EXECUTABLE(TestFramework ${TestFrameworkSource})
#gtest_discover_tests(TestFramework)
add_dependencies(TestFramework ExamplePlugin)

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

SET(LinkedLibrary $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(LinkedLibrary)
SET(LinkedLibrary ${LinkedLibrary} chaos_common gtest)
IF (USE_GPT)
    MESG("Link google performance tools")
    SET(LinkedLibrary ${LinkedLibrary} tcmalloc)
ENDIF()
IF(CLING_VIRTUAL_MACHINE_ENABLE)
    MESG("Link cling interpreter")
    SET(LinkedLibrary ${LinkedLibrary} clingInterpreter)
ENDIF()

TARGET_LINK_LIBRARIES(TestFramework ${LinkedLibrary})

INSTALL_TARGETS(/bin TestFramework)
