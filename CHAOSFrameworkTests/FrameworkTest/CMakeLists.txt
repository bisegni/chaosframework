cmake_minimum_required(VERSION 2.8)
project(TestFramework)

SET(TestFrameworkSource main.cpp
                        plugin/TestPlugin.cpp
                        script/ScriptTest.cpp
                        data/TestVariant.cpp
                        data/TestCDataWrapper.cpp
                        external_unit/ExternalUnitTest.cpp
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
                        trigger/TriggerTest.cpp)

INCLUDE_DIRECTORIES(. ../../usr/local/include)
LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${PROJECT_SOURCE_DIR}/../../usr/local/lib )
if(UNIX AND NOT APPLE)
      LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../usr/local/lib64)
      LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../usr/local/lib/x86_64-linux-gnu)
endif()
MESG("Link directory ${PROJECT_SOURCE_DIR}/../../usr/local/lib")
ADD_EXECUTABLE(TestFramework ${TestFrameworkSource})

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

SET(LinkedLibrary $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(LinkedLibrary)

TARGET_LINK_LIBRARIES(TestFramework gtest chaos_common ${LinkedLibrary})

INSTALL_TARGETS(/bin TestFramework)
