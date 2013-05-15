SET(driver_lib_src attribute.cpp basichw.cpp CommunicationChannel.cpp device_proxy.cpp ModbusProtocol.cpp  ProtocolDriver.cpp  SWAlim.cpp TcpChannel.cpp)

SET(common_lib_library boost_program_options boost_system boost_thread boost_chrono boost_regex boost_log boost_log_setup memcached msgpack msgpack-rpc mpio)


ADD_LIBRARY(chaosDD SHARED ${driver_lib_src})

set_target_properties(chaosDD PROPERTIES VERSION 1.0.0 SOVERSION 1) 

TARGET_LINK_LIBRARIES(chaosDD ${common_lib_library}) 

        
FILE(GLOB files "*.h")
INSTALL(FILES ${files} DESTINATION include/chaos/dd)

INSTALL(TARGETS chaosDD
        DESTINATION "lib"
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
