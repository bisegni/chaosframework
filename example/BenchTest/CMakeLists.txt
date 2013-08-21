cmake_minimum_required(VERSION 2.6)
option(BUILD_FORCE_32 "Set to ON to enable 32 bit compilation" OFF)

IF( ($ENV{CHAOS32}) OR (BUILD_FORCE_32) )
    MESSAGE(STATUS "Enabling 32 bit Compilation")
    set (CMAKE_C_FLAGS "-m32")
    set (CMAKE_CXX_FLAGS "-m32")
    set (CMAKE_LINK_FLAGS "-m32")
ENDIF()

SET(reactor_device_exe_src main_reactor.cpp Batch_Reactor.cpp BenchTestCU.cpp Disturbo.cpp  WriteFile.cpp)
SET(reactor_controller_exe_src main_controller.cpp Batch_Controller.cpp ReactorController.cpp MainOrkestrator.cpp WriteFile.cpp)

INCLUDE_DIRECTORIES(. ${PROJECT_SOURCE_DIR}/../../usr/local/include /usr/local/include /usr/include) 
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../../usr/local/lib /usr/local/lib)

ADD_EXECUTABLE(BenchTestReactor ${reactor_device_exe_src})
ADD_EXECUTABLE(BenchTestController ${reactor_controller_exe_src})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(BenchTestReactor pthread chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
	TARGET_LINK_LIBRARIES(BenchTestController pthread chaos_uitoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(BenchTestReactor chaos_cutoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
	TARGET_LINK_LIBRARIES(BenchTestController chaos_uitoolkit chaos_common boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup boost_atomic memcached msgpack msgpack-rpc mpio)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

INSTALL_TARGETS(/bin BenchTestReactor)
INSTALL_TARGETS(/bin BenchTestController)