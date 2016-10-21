project(chaos_uitoolkit)
INCLUDE_DIRECTORIES(chaos/ui_toolkit ../../)
SET(ui_lib_src ChaosUIToolkit.cpp ChaosUIToolkitCWrapper.cc
				HighLevelApi/DeviceController.cpp HighLevelApi/HLDataApi.cpp
				HighLevelApi/HLInfrastructureApi.cpp LowLevelApi/LLDataApi.cpp LowLevelApi/LLRpcApi.cpp)

# IF(BUILD_FORCE_STATIC)
# ADD_LIBRARY(${PROJECT_NAME} STATIC ${ui_lib_src})
# ELSE()
# ADD_LIBRARY(${PROJECT_NAME} SHARED ${ui_lib_src})
# set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0.0 SOVERSION 1)
# ENDIF()

ADD_LIBRARY(${PROJECT_NAME} ${ui_lib_src})
#Set_target_properties(${PROJECT_NAME}_static PROPERTIES output_name chaos_uitoolkit) 


SET(chaos_uitoolkit_lib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(chaos_uitoolkit_lib)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  chaos_common ${chaos_uitoolkit_lib})



FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit)

FILE(GLOB files "Common/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit/Common)

FILE(GLOB files "HighLevelApi/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit/HighLevelApi)

FILE(GLOB files "LowLevelApi/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit/LowLevelApi)

INSTALL(TARGETS ${PROJECT_NAME}
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
# INSTALL(TARGETS ${PROJECT_NAME}_static
#         DESTINATION "lib"
#         PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
