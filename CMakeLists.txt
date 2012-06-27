cmake_minimum_required (VERSION 2.6)
project (chaos)


EXECUTE_PROCESS(COMMAND chaos/common/version.sh)

INCLUDE_DIRECTORIES(.) 
ADD_DEFINITIONS(-g -O2 -Wall)

ADD_SUBDIRECTORY(chaos/common bin_common)
ADD_SUBDIRECTORY(chaos/cu_toolkit bin_cutoolkit)
ADD_SUBDIRECTORY(chaos/ui_toolkit bin_uitoolkit)
ADD_SUBDIRECTORY(example/ControlUnitTest bin_example_cutest)
ADD_SUBDIRECTORY(example/UIToolkitCMDLineExample bin_example_uitest)
