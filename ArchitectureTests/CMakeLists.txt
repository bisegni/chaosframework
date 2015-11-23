cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
INCLUDE_DIRECTORIES(../)

SET(ArchitectureTests main.cpp)
SET(ArchitectureTests
    ${ArchitectureTests}
    utility/HashMapTest.cpp)
SET(ArchitectureTests
    ${ArchitectureTests}
    network/FeederTest.cpp)
SET(ArchitectureTests
    ${ArchitectureTests}
    thread/ObjectQueueTest.cpp
    thread/ObjectPriorityQueueTest.cpp)

ADD_EXECUTABLE(ArchitectureTests ${ArchitectureTests})

SET(ArchitectureTestsLib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(ArchitectureTestsLib)
SET(ChaosNodeDirectoryLib chaos_common ${ArchitectureTestsLib})


IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ArchitectureTests pthread dl ${ArchitectureTestsLib})
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ArchitectureTests dl ${ArchitectureTestsLib})
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

INSTALL_TARGETS(/bin ArchitectureTests)
