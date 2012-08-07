cmake_minimum_required (VERSION 2.6)
project (chaos)

SET( ${PROJECT_NAME}_MAJOR_VERSION 0 ) 
SET( ${PROJECT_NAME}_MINOR_VERSION 1 ) 
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )

OPTION( BUILD_FORCE_32 "Force 32 bit compilation" OFF )

EXECUTE_PROCESS(COMMAND chaos/common/version.sh)

INCLUDE_DIRECTORIES(.) 
ADD_DEFINITIONS(-g -O2 -Wall)
    
IF( BUILD_FORCE_32 ) 
    MESSAGE( STATUS "Forcing 32 bit compilation" )
    set (CMAKE_C_FLAGS "-m32 -march=i686")
    set (CMAKE_CXX_FLAGS "-m32 -march=i686")
    set (CMAKE_LINK_FLAGS "-m32 -march=i686")
ENDIF()

ADD_SUBDIRECTORY(chaos/common bin_common)
ADD_SUBDIRECTORY(chaos/cu_toolkit bin_cutoolkit)
ADD_SUBDIRECTORY(chaos/ui_toolkit bin_uitoolkit)
ADD_SUBDIRECTORY(example/ControlUnitTest bin_example_cutest)
ADD_SUBDIRECTORY(example/UIToolkitCMDLineExample bin_example_uitest)
