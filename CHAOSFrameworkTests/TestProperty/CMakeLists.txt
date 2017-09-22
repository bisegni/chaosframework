cmake_minimum_required(VERSION 2.8)
project(FrameworkTest)

SET(src main.cpp
        templated_key_value_hashmap/TemplateKeyValueHashMap.cpp
        trigger/TriggerTest.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${src})

IF(BUILD_FORCE_STATIC)
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} gtest chaos_common ${FrameworkLib})

INSTALL_TARGETS(/bin ${PROJECT_NAME})

add_test (NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
