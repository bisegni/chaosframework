cmake_minimum_required(VERSION 2.6)
IF (DEFINED ENV{CHAOS_BUNDLE} )
  include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
ENDIF()

SET(ArchitectureTestsSrc main.cpp)
SET(ArchitectureTestsSrc
    ${ArchitectureTestsSrc}
    utility/HashMapTest.cpp
    utility/StateFlagCatalogTest.cpp
    )
SET(ArchitectureTestsSrc
    ${ArchitectureTestsSrc}
    network/FeederTest.cpp)
SET(ArchitectureTestsSrc
    ${ArchitectureTestsSrc}
    thread/ObjectQueueTest.cpp
    thread/ObjectPriorityQueueTest.cpp
    queue/PriorityTest.cpp)


ADD_EXECUTABLE(ArchitectureTests ${ArchitectureTestsSrc})

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()
TARGET_LINK_LIBRARIES(ArchitectureTests chaos_common ${FrameworkLib})
INSTALL_TARGETS(/bin ArchitectureTests)
