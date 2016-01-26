cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

SET(ArchitectureTestsSrc main.cpp)
SET(ArchitectureTestsSrc
    ${ArchitectureTestsSrc}
    utility/HashMapTest.cpp)
SET(ArchitectureTestsSrc
    ${ArchitectureTestsSrc}
    network/FeederTest.cpp)
SET(ArchitectureTestsSrc
    ${ArchitectureTestsSrc}
    thread/ObjectQueueTest.cpp
    thread/ObjectPriorityQueueTest.cpp
    thread/PriorityTest.cpp)


ADD_EXECUTABLE(ArchitectureTests ${ArchitectureTestsSrc})

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()
TARGET_LINK_LIBRARIES(ArchitectureTests chaos_common ${FrameworkLib})
INSTALL_TARGETS(/bin ArchitectureTests)
