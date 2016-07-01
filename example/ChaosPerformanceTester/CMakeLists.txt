cmake_minimum_required(VERSION 2.6)
IF (DEFINED ENV{CHAOS_BUNDLE} )
  include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)
ENDIF()

SET(chaos_perf_tester_src main.cpp)


ADD_EXECUTABLE(ChaosPerformanceTester ${chaos_perf_tester_src})

SET(ChaosPerformanceTesterLib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(ChaosPerformanceTesterLib)

IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

TARGET_LINK_LIBRARIES(ChaosPerformanceTester chaos_uitoolkit chaos_common ${FrameworkLib})

INSTALL_TARGETS(/bin ChaosPerformanceTester)