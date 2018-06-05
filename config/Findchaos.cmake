set(PREFIX ${CMAKE_CURRENT_LIST_DIR})
set(chaos_INCLUDE_DIRS ${PREFIX}/include)
FILE(GLOB boost_libs ${PREFIX}/lib/libboost*.a)
ADD_DEFINITIONS(-DCHAOS)
set(chaos_LIBRARIES chaos_cutoolkit chaos_common)
set(chaos_client_LIBRARIES chaos_metadata_service_client)
SET(CMAKE_INSTALL_RPATH "${PREFIX}/lib")
link_directories(${PREFIX}/lib)
SET(FrameworkLib ${chaos_LIBRARIES} ${boost_libs} pthread dl)
include(${PREFIX}/CMakeMacroUtils.txt)
