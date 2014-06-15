SET(cnd_src main.cpp ChaosNodeDirectory.cpp)

ADD_EXECUTABLE(ChaosNodeDirectory ${cnd_src})

SET(ChaosNodeDirectoryLib $ENV{CHAOS_LINK_LIBRARY})
separate_arguments(ChaosNodeDirectoryLib)
SET(ChaosNodeDirectoryLib chaos_common ${ChaosNodeDirectoryLib})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosNodeDirectory pthread dl ${ChaosNodeDirectoryLib})
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	TARGET_LINK_LIBRARIES(ChaosNodeDirectory dl ${ChaosNodeDirectoryLib})
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


INSTALL_TARGETS(/bin ChaosNodeDirectory)