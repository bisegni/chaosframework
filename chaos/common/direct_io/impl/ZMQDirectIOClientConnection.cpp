//
//  ZMQDirectIOClientConnection.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 10/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos/common/direct_io/impl/ZMQDirectIOClientConnection.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/direct_io/impl/ZMQDirectIOClient.h>


#include <boost/format.hpp>

#include <string.h>


typedef boost::unique_lock<boost::shared_mutex>	ZMQDirectIOClientConnectionWriteLock;
typedef boost::shared_lock<boost::shared_mutex> ZMQDirectIOClientConnectionReadLock;

using namespace chaos::common::direct_io::impl;


#define INFO    INFO_LOG(ZMQDirectIOClientConnection)
#define DBG     DBG_LOG(ZMQDirectIOClientConnection)
#define ERR     ERR_LOG(ZMQDirectIOClientConnection)

ZMQDirectIOClientConnection::ZMQDirectIOClientConnection(void *_zmq_context,
                                                         const std::string& server_description,
                                                         uint16_t endpoint):
DirectIOClientConnection(server_description,
                         endpoint),
zmq_context(_zmq_context),
socket_priority(NULL),
socket_service(NULL),
message_counter(0) {
    monitor_info.monitor_socket = NULL;
    default_configuration["ZMQ_LINGER"] = "0";
    default_configuration["ZMQ_RCVHWM"] = "3";
    default_configuration["ZMQ_SNDHWM"] = "3";
    default_configuration["ZMQ_RCVTIMEO"] = boost::lexical_cast<std::string>(DirectIOConfigurationKey::GlobalDirectIOTimeoutinMSec);
    default_configuration["ZMQ_SNDTIMEO"] = boost::lexical_cast<std::string>(DirectIOConfigurationKey::GlobalDirectIOTimeoutinMSec);
    default_configuration["ZMQ_RECONNECT_IVL"] = "5000";
    default_configuration["ZMQ_RECONNECT_IVL_MAX"] = "10000";
}

ZMQDirectIOClientConnection::~ZMQDirectIOClientConnection() {}

void ZMQDirectIOClientConnection::init(void *init_data)  {
    int err = 0;
    if((err = ensureSocket()) != 0) throw CException(err, "Error configuring socket", __PRETTY_FUNCTION__);
}

void ZMQDirectIOClientConnection::deinit()  {
    //disable monitor
    releaseSocketPair();
    INFO << "Disabled monitor socket for " << getServerDescription();
}

//------------------------------STATIC METHOD---------------------------------
int ZMQDirectIOClientConnection::readMonitorMesg(void *monitor,
                                                 int *value,
                                                 char *address,
                                                 int address_max_size) {
    // First frame in message contains event number and value
    zmq_msg_t msg;
    zmq_msg_init (&msg);
    if (zmq_msg_recv (&msg, monitor, 0) == -1)
        return -1; // Interruped, presumably
    assert (zmq_msg_more (&msg));
    
    uint8_t *data = (uint8_t *) zmq_msg_data (&msg);
    uint16_t event = *(uint16_t *) (data);
    if (value)
        *value = *(uint32_t *) (data + 2);
    
    // Second frame in message contains event address
    zmq_msg_init (&msg);
    if (zmq_msg_recv (&msg, monitor, 0) == -1)
        return -1; // Interruped, presumably
    assert (!zmq_msg_more (&msg));
    
    if (address) {
        uint8_t *data = (uint8_t *) zmq_msg_data (&msg);
        size_t size = zmq_msg_size (&msg);
        size_t size_to_use = (size>address_max_size?address_max_size:size);
        memcpy (address, data, size_to_use);
        address [size_to_use] = 0;
    }
    return event;
}

void ZMQDirectIOClientConnection::socketMonitor() {
    const int linger_period = 0;
    char addr[1025];
    int event = 0;
    int value = 0;
    int rc = 0;
    //std::vector<std::string> server_desc_tokens;
    DEBUG_CODE(DBG << "Start monitor for " << monitor_info.monitor_url;)
    
    monitor_info.monitor_socket = zmq_socket (zmq_context, ZMQ_PAIR);
    if(monitor_info.monitor_socket == NULL) return;
    rc = zmq_setsockopt (monitor_info.monitor_socket, ZMQ_LINGER, &linger_period, sizeof(int));
    
    rc = zmq_connect (monitor_info.monitor_socket, monitor_info.monitor_url.c_str());
    if(rc) return;
    while (monitor_info.run) {
        event = readMonitorMesg(monitor_info.monitor_socket,
                                &value,
                                addr,
                                1025);
        switch (event) {
            case ZMQ_EVENT_CONNECTED:
                DEBUG_CODE(DBG << "ZMQ_EVENT_CONNECTED to " << getServerDescription();)
                lowLevelManageEvent(DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected);
                break;
            case ZMQ_EVENT_CONNECT_DELAYED:
                DEBUG_CODE(DBG << "ZMQ_EVENT_CONNECT_DELAYED " << getServerDescription();)
                break;
            case ZMQ_EVENT_CONNECT_RETRIED:
                DEBUG_CODE(DBG << "ZMQ_EVENT_CONNECT_RETRIED " << getServerDescription();)
                break;
            case ZMQ_EVENT_CLOSE_FAILED:
                lowLevelManageEvent(DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected);
                DEBUG_CODE(DBG << "ZMQ_EVENT_CLOSE_FAILED " << getServerDescription();)
                break;
            case ZMQ_EVENT_CLOSED:
                lowLevelManageEvent(DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected);
                DEBUG_CODE(DBG << "ZMQ_EVENT_CLOSED " << getServerDescription();)
                break;
            case ZMQ_EVENT_DISCONNECTED:
                lowLevelManageEvent(DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected);
                DEBUG_CODE(DBG << "ZMQ_EVENT_DISCONNECTED " << getServerDescription();)
                break;
            case ZMQ_EVENT_MONITOR_STOPPED:
                monitor_info.run = false;
                DEBUG_CODE(DBG << "ZMQ_EVENT_MONITOR_STOPPED " << getServerDescription();)
                break;
        }
    }
    zmq_close(monitor_info.monitor_socket);
    monitor_info.monitor_socket = NULL;
    DEBUG_CODE(DBG << "End monitor for " << monitor_info.monitor_url;)
}

//! get new connection
int ZMQDirectIOClientConnection::getNewSocketPair() {
    int err = 0;
    std::string socket_id;
    std::string url;
    std::string error_str;
    std::vector<std::string> resolved_ip;
    
    try {
        
        DEBUG_CODE(DBG << "Allocating priority socket";)
        socket_priority = zmq_socket (zmq_context, ZMQ_DEALER);
        if(socket_priority == NULL) throw chaos::CException(1, "Error creating priority socket", __FUNCTION__);
        
        if((err = ZMQBaseClass::configureSocketWithStartupParameter(socket_priority,
                                                                    default_configuration,
                                                                    chaos::GlobalConfiguration::getInstance()->getDirectIOClientImplKVParam(),
                                                                    "ZMQ DirectIO Client service socket"))) {
            throw chaos::CException(2, "Error configuring service socket", __FUNCTION__);
        }
        
        //---------------------------------------------------------------------------------------------------------------
        DEBUG_CODE(DBG << "Allocating service socket";)
        socket_service = zmq_socket (zmq_context, ZMQ_DEALER);
        if(socket_service == NULL) throw chaos::CException(2, "Error creating service socket", __FUNCTION__);
        if((err = ZMQBaseClass::configureSocketWithStartupParameter(socket_service,
                                                                    default_configuration,
                                                                    chaos::GlobalConfiguration::getInstance()->getDirectIOClientImplKVParam(),
                                                                    "ZMQ DirectIO Client service socket"))) {
            throw chaos::CException(2, "Error configuring service socket", __FUNCTION__);
        }
        
        err = setAndReturnID(socket_priority,
                             priority_identity);
        if(err) throw chaos::CException(err, "Error setting identity on priority socket", __FUNCTION__);
        
        err = setAndReturnID(socket_service,
                             service_identity);
        if(err) throw chaos::CException(err, "Error setting identity on service socket", __FUNCTION__);
        
        //set the server information on socket
        decoupleServerDescription(server_description, priority_endpoint, service_endpoint);
        
        //allocate monitor
        monitor_info.run = true;
        monitor_info.monitor_socket = NULL;
        monitor_info.unique_identification = getUniqueUUID();
        monitor_info.monitor_url = boost::str( boost::format("inproc://%1%") % getUniqueUUID());
        monitor_info.monitor_thread.reset(new boost::thread(boost::bind(&ZMQDirectIOClientConnection::socketMonitor, this)));
        
        err = zmq_socket_monitor(socket_priority, monitor_info.monitor_url.c_str(), ZMQ_EVENT_ALL);
        if(err) throw chaos::CException(err, "Error activating monitor on service socket", __FUNCTION__);
        
        url = boost::str( boost::format("tcp://%1%") % priority_endpoint);
        DEBUG_CODE(DBG << "connect to priority endpoint " << url;)
        err = zmq_connect(socket_priority, url.c_str());
        if(err) {
            error_str = boost::str( boost::format("Error connecting priority socket to %1%") % priority_endpoint);
            throw chaos::CException(err, error_str, __FUNCTION__);
        }
        
        //add monitor on priority socket
        url = boost::str( boost::format("tcp://%1%") % service_endpoint);
        DEBUG_CODE(DBG << "connect to service endpoint " << url;)
        err = zmq_connect(socket_service, url.c_str());
        if(err) {
            error_str = boost::str( boost::format("Error connecting service socket to %1%") % service_endpoint);
            throw chaos::CException(err, error_str, __FUNCTION__);
        }
    } catch(chaos::CException& ex) {
        releaseSocketPair();
        err = ex.errorCode;
        DECODE_CHAOS_EXCEPTION(ex);
    }
    return err;
}

//! release an instantiated connection
int ZMQDirectIOClientConnection::releaseSocketPair() {
    int err = 0;
    //CHAOS_ASSERT(conn->monitor_info)
    //stop the monitor
    INFO << "Release the sockets for: " << getServerDescription();
    err = zmq_socket_monitor(socket_priority, NULL, 0);
    if(err) ERR << "Error disconnecting priority socket for " << getServerDescription() << " from monitor";
    
    if(monitor_info.monitor_thread.get()) {
        if(monitor_info.monitor_thread->joinable())
            monitor_info.monitor_thread->join();
    }
    
    if(socket_priority) {
        err = zmq_close(socket_priority);
        if(err) ERR << "Error disconnecting priority socket for " << getServerDescription();
        socket_priority = NULL;
    }
    if(socket_service) {
        err = zmq_close(socket_service);
        if(err) ERR << "Error closing service socket for " << getServerDescription();
        socket_service = NULL;
    }
    return err;
}
int ZMQDirectIOClientConnection::sendPriorityData(chaos::common::direct_io::DirectIODataPackSPtr data_pack) {
    CHAOS_ASSERT(data_pack);
    boost::unique_lock<boost::shared_mutex> wl(mutext_send_message);
    completeDataPack(*data_pack);
    return writeToSocket(socket_priority,
                         priority_identity,
                         MOVE(data_pack));
}
// send the data to the server layer on priority channel
int ZMQDirectIOClientConnection::sendPriorityData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                                  chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) {
    int err = 0;
    CHAOS_ASSERT(data_pack);
    boost::unique_lock<boost::shared_mutex> wl(mutext_send_message);
    completeDataPack(*data_pack);
    err = writeToSocket(socket_priority,
                        priority_identity,
                        MOVE(data_pack),
                        synchronous_answer);
    if(err > 0 /*resource not available*/) {
        //change id ofr socket
        if((ZMQBaseClass::setAndReturnID(socket_priority,
                                         service_identity))) {
            ERR << "Error configuring new id for socker :" << service_endpoint;
        } else {
            ZMQBaseClass::resetOutputQueue(socket_service,
                                           default_configuration,
                                           chaos::GlobalConfiguration::getInstance()->getDirectIOClientImplKVParam());

        }
    }
    return err;
}

int ZMQDirectIOClientConnection::sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack) {
    CHAOS_ASSERT(data_pack);
    boost::unique_lock<boost::shared_mutex> wl(mutext_send_message);
    completeDataPack(*data_pack);
    return writeToSocket(socket_service,
                         service_identity,
                         MOVE(data_pack));
}

// send the data to the server layer on the service channel
int ZMQDirectIOClientConnection::sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                                 chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) {
    int err = 0;
    CHAOS_ASSERT(data_pack);
    boost::unique_lock<boost::shared_mutex> wl(mutext_send_message);
    completeDataPack(*data_pack);
    err = writeToSocket(socket_service,
                        service_identity,
                        MOVE(data_pack),
                        synchronous_answer);
    if(err > 0 /*resource not available*/) {
        //change id ofr socket
//        if((ZMQBaseClass::setAndReturnID(socket_service,
//                                         service_identity))) {
//            ERR << "Error configuring new id for socker :" << service_endpoint;
//        } else {
//            ZMQBaseClass::resetOutputQueue(socket_service,
//                                           default_configuration,
//                                           chaos::GlobalConfiguration::getInstance()->getDirectIOClientImplKVParam());
//
//        }
    }
    return err;
}

bool ZMQDirectIOClientConnection::ensureSocket() {
    int err = 0;
    if(socket_service == NULL ||
       socket_priority == NULL) {
        err = getNewSocketPair();
    }
    return err;
}
//send data with zmq tech
int ZMQDirectIOClientConnection::writeToSocket(void *socket,
                                               std::string& identity,
											   chaos::common::direct_io::DirectIODataPackSPtr data_pack) {
    CHAOS_ASSERT(socket && data_pack);
    CHAOS_ASSERT(data_pack->header.dispatcher_header.fields.synchronous_answer == false);
    int err = 0;
    if((err = sendDatapack(socket,
                           MOVE(data_pack)))) {
        ERR << "Error sending datapack with code:" << err;
    }
    return err;
}
//send data with zmq tech
int ZMQDirectIOClientConnection::writeToSocket(void *socket,
                                               std::string& identity,
                                               chaos::common::direct_io::DirectIODataPackSPtr data_pack,
											   chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) {
    CHAOS_ASSERT(socket && data_pack);
    CHAOS_ASSERT(data_pack->header.dispatcher_header.fields.synchronous_answer);
    uint16_t current_counter = data_pack->header.dispatcher_header.fields.counter = message_counter++;
    int err = 0;
    if((err = sendDatapack(socket,
                           MOVE(data_pack)))) {
        ERR << "Error sending datapack with code:" << err;
    } else {
        //we need an aswer
        do {
            if((err = reveiceDatapack(socket,
                                      synchronous_answer))) {
                ERR << "Error receiving answer datapack with code:" << err;
            }
        } while (err == 0 &&
                 synchronous_answer->header.dispatcher_header.fields.counter < current_counter);
    }
    return err;
}
