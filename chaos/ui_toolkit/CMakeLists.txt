SET(ui_lib_src ChaosUIToolkit.cpp ChaosUIToolkitCWrapper.cc
				HighLevelApi/DeviceController.cpp HighLevelApi/HLDataApi.cpp
				HighLevelApi/HLInfrastructureApi.cpp LowLevelApi/LLDataApi.cpp LowLevelApi/LLRpcApi.cpp)

IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(chaos_uitoolkit STATIC ${ui_lib_src})
ELSE()
ADD_LIBRARY(chaos_uitoolkit SHARED ${ui_lib_src})
ENDIF()
#set_target_properties(chaos_uitoolkit_static PROPERTIES output_name chaos_uitoolkit) 


SET(chaos_uitoolkit_lib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(chaos_uitoolkit_lib)
TARGET_LINK_LIBRARIES(chaos_uitoolkit  chaos_common ${chaos_uitoolkit_lib})



FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit)

FILE(GLOB files "Common/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit/Common)

FILE(GLOB files "HighLevelApi/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit/HighLevelApi)

FILE(GLOB files "LowLevelApi/*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/ui_toolkit/LowLevelApi)

INSTALL(TARGETS chaos_uitoolkit
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
# INSTALL(TARGETS chaos_uitoolkit_static
#         DESTINATION "lib"
#         PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)