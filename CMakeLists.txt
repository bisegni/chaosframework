cmake_minimum_required (VERSION 2.6)
project (chaos)

SET( ${PROJECT_NAME}_MAJOR_VERSION 0 ) 
SET( ${PROJECT_NAME}_MINOR_VERSION 1 ) 
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )

OPTION( BUILD_FORCE_32 "Force 32 bit compilation" OFF )


EXECUTE_PROCESS(COMMAND chaos/common/version.sh)

INCLUDE_DIRECTORIES(. ${CMAKE_INSTALL_PREFIX}/usr/local/include) 
ADD_DEFINITIONS(-g -O2 -Wall)
    
IF( BUILD_FORCE_32 ) 
    MESSAGE( STATUS "Forcing 32 bit compilation" )
    set (CMAKE_C_FLAGS "-m32 -march=i686")
    set (CMAKE_CXX_FLAGS "-m32 -march=i686")
    set (CMAKE_LINK_FLAGS "-m32 -march=i686")
ENDIF()

message(STATUS "Configure Common Layer")
ADD_SUBDIRECTORY(chaos/common bin/bin_common)
message(STATUS "Configure CUToolkit Layer")
ADD_SUBDIRECTORY(chaos/cu_toolkit bin/bin_cutoolkit)
message(STATUS "Configure UIToolkit Layer")
ADD_SUBDIRECTORY(chaos/ui_toolkit bin/bin_uitoolkit)
message(STATUS "Configure Control Unit Test example")
ADD_SUBDIRECTORY(example/ControlUnitTest bin/bin_example_cutest)
message(STATUS "Configure UIToolkit command line example")
ADD_SUBDIRECTORY(example/UIToolkitCMDLineExample bin/bin_example_uicmdline_test)
message(STATUS "Configure UIToolkit c-wrapper command line example")
ADD_SUBDIRECTORY(example/UIToolkitCMDLineCWrapperExample bin/bin_example_uicmdline_cwrapper_test)
