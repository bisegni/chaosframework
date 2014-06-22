cmake_minimum_required(VERSION 2.8)
project (chaos)

SET( ${PROJECT_NAME}_MAJOR_VERSION 0 )
SET( ${PROJECT_NAME}_MINOR_VERSION 1 )
SET( ${PROJECT_NAME}_PATCH_LEVEL 0 )

add_definitions(-DBOOST_ALL_DYN_LINK)
add_definitions(-DDEBUG)


INCLUDE_DIRECTORIES(. ${PROJECT_SOURCE_DIR}/../usr/local/include /Library/Java/JavaVirtualMachines/jdk1.8.0_05.jdk/Contents/Home/include /Library/Java/JavaVirtualMachines/jdk1.8.0_05.jdk/Contents/Home/include/darwin /usr/local/include /usr/include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/../usr/local/lib /usr/local/lib /usr/lib)
ADD_DEFINITIONS(-O3 )

SET(jnichaos_src JNIChaos.c)

ADD_LIBRARY(jnichaos SHARED ${jnichaos_src})
set_target_properties(jnichaos PROPERTIES VERSION 1.0.0 SOVERSION 1)

SET(jnichaos_lib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(jnichaos_lib)
TARGET_LINK_LIBRARIES(jnichaos chaos_uitoolkit  chaos_common ${jnichaos_lib})


INSTALL(TARGETS jnichaos
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
