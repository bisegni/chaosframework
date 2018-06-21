cmake_minimum_required(VERSION 2.8)
project(TestFramework)

SET(TestFrameworkSource main.cpp
                        plugin/TestPlugin.cpp
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

SET(TestFrameworkSource ${TestFrameworkSource}
                        script/ScriptTest.cpp
                        script/ScriptClingTest.cpp)
INCLUDE_DIRECTORIES(. ../../usr/local/include)
LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${PROJECT_SOURCE_DIR}/../../usr/local/lib )
if(UNIX AND NOT APPLE)
      LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../usr/local/lib64)
      LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../usr/local/lib/x86_64-linux-gnu)
endif()
ADD_EXECUTABLE(TestFramework ${TestFrameworkSource})

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
IF (CLING_VIRTUAL_MACHINE)
    MESG("Link cling interpreter")
    SET(LinkedLibrary ${LinkedLibrary} clingInterpreter)
ENDIF()

TARGET_LINK_LIBRARIES(TestFramework ${LinkedLibrary})

INSTALL_TARGETS(/bin TestFramework)
