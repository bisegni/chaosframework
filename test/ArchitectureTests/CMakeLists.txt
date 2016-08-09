cmake_minimum_required(VERSION 2.8)
project(ArchitectureTests)

SET(src main.cpp)
SET(src
    ${src}
    utility/HashMapTest.cpp
    utility/StateFlagCatalogTest.cpp
    )
SET(src
    ${src}
    network/FeederTest.cpp)
SET(src
    ${src}
    thread/ObjectQueueTest.cpp
    thread/ObjectPriorityQueueTest.cpp
    queue/PriorityTest.cpp)
SET(src
    ${src}
    trigger/TestEvent.cpp)

ADD_EXECUTABLE(${PROJECT_NAME} ${src})

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} chaos_common ${FrameworkLib})
INSTALL_TARGETS(/bin ${PROJECT_NAME})

add_test (NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
