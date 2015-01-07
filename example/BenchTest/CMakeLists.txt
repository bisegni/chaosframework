
cmake_minimum_required(VERSION 2.6)
include($ENV{CHAOS_BUNDLE}/tools/project_template/CMakeChaos.txt)

SET(reactor_device_exe_src main_reactor.cpp Batch_Reactor.cpp BenchTestCU.cpp Disturbo.cpp  WriteFile.cpp)
SET(reactor_controller_exe_src main_controller.cpp Batch_Controller.cpp ReactorController.cpp MainOrkestrator.cpp WriteFile.cpp)

ADD_EXECUTABLE(BenchTestReactor ${reactor_device_exe_src})
ADD_EXECUTABLE(BenchTestController ${reactor_controller_exe_src})
IF(BUILD_FORCE_STATIC)
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
  TARGET_LINK_LIBRARIES(BenchTestReactor chaos_cutoolkit ${FrameworkLib})
  TARGET_LINK_LIBRARIES(BenchTestController chaos_uitoolkit chaos_cutoolkit ${FrameworkLib})
ELSE()
    TARGET_LINK_LIBRARIES(BenchTestReactor chaos_cutoolkit chaos_common ${FrameworkLib})
  TARGET_LINK_LIBRARIES(BenchTestController chaos_uitoolkit chaos_cutoolkit chaos_common ${FrameworkLib})

ENDIF()

INSTALL_TARGETS(/bin BenchTestReactor)
INSTALL_TARGETS(/bin BenchTestController)