SET(ui_lib_src ChaosUIToolkit.cpp ChaosUIToolkitCWrapper.cc HighLevelApi/DeviceController.cpp HighLevelApi/HLDataApi.cpp HighLevelApi/HLInfrastructureApi.cpp LowLevelApi/LLDataApi.cpp LowLevelApi/LLRpcApi.cpp)

ADD_LIBRARY(chaos_uitoolkit SHARED ${ui_lib_src})
#set_target_properties(chaos_uitoolkit PROPERTIES VERSION 1.0.0 SOVERSION 1) 

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