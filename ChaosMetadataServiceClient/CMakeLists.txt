SET(mds_client_lib_src ChaosMetadataServiceClient.cpp)

IF(BUILD_FORCE_STATIC)
ADD_LIBRARY(chaos_metadata_service_client STATIC ${mds_client_lib_src})
ELSE()
ADD_LIBRARY(chaos_metadata_service_client SHARED ${mds_client_lib_src})
ENDIF()


SET(chaos_metadata_service_client_lib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(chaos_metadata_service_client_lib)
TARGET_LINK_LIBRARIES(chaos_metadata_service_client  chaos_common ${chaos_metadata_service_client_lib})



FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/metadata_service_client)


INSTALL(TARGETS chaos_metadata_service_client
DESTINATION "lib"
PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)