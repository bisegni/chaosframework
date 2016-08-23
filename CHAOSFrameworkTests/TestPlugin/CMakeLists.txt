cmake_minimum_required(VERSION 2.6)
project(TestPlugin)

SET(loadr_src main)

INCLUDE_DIRECTORIES(. ${PROJECT_SOURCE_DIR}/usr/local/include /usr/local/include /usr/include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/usr/local/lib /usr/local/lib)

ADD_EXECUTABLE(TestPlugin ${loadr_src})

IF(BUILD_FORCE_STATIC)
SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--whole-archive -lchaos_common -Wl,--no-whole-archive")
ENDIF()

SET(TestPluginLib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(TestPluginLib)


TARGET_LINK_LIBRARIES(TestPlugin chaos_cutoolkit chaos_common ${TestPluginLib})

INSTALL_TARGETS(/bin TestPlugin)
MESG("installation prefix ${CMAKE_INSTALL_PREFIX}/bin")
add_test (NAME ${PROJECT_NAME}
            COMMAND ${PROJECT_NAME}
            WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin")
