cmake_minimum_required(VERSION 2.6)

SET(source main.cpp)
INCLUDE_DIRECTORIES( ../)
ADD_EXECUTABLE(ChaosMicroUnitToolkitTest ${source})

IF(BUILD_FORCE_STATIC)
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(ChaosMicroUnitToolkitTest chaos_micro_unit_toolkit)
INSTALL_TARGETS(/bin ChaosMicroUnitToolkitTest)
