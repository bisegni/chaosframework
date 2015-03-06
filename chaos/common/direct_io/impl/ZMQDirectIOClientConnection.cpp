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

#define ZMQDIO_CONNECTION_LOG_HEAD "[ZMQDirectIOClientConnection] - "

#define ZMQDIO_CONNECTION_LAPP_ LAPP_ << ZMQDIO_CONNECTION_LOG_HEAD
#define ZMQDIO_CONNECTION_LDBG_ LDBG_ << ZMQDIO_CONNECTION_LOG_HEAD << __FUNCTION__ << " - "
#define ZMQDIO_CONNECTION_LERR_ LERR_ << ZMQDIO_CONNECTION_LOG_HEAD << __FUNCTION__ << "(" << __LINE__ << ") - "

ZMQDirectIOClientConnection::ZMQDirectIOClientConnection(std::string server_description, void *_socket_priority, void *_socket_service, uint16_t endpoint):
DirectIOClientConnection(server_description, endpoint),
socket_priority(_socket_priority),
socket_service(_socket_service),
monitor_info(NULL) {
}


ZMQDirectIOClientConnection::~ZMQDirectIOClientConnection() {
    
}

// send the data to the server layer on priority channel
int64_t ZMQDirectIOClientConnection::sendPriorityData(DirectIODataPack *data_pack,
                                                      DirectIOClientDeallocationHandler *header_deallocation_handler,
                                                      DirectIOClientDeallocationHandler *data_deallocation_handler,
                                                      DirectIOSynchronousAnswer **synchronous_answer) {
    return writeToSocket(socket_priority,
                         priority_identity,
                         completeDataPack(data_pack),
                         header_deallocation_handler,
                         data_deallocation_handler,
                         synchronous_answer);
}

// send the data to the server layer on the service channel
int64_t ZMQDirectIOClientConnection::sendServiceData(DirectIODataPack *data_pack,
                                                     DirectIOClientDeallocationHandler *header_deallocation_handler,
                                                     DirectIOClientDeallocationHandler *data_deallocation_handler,
                                                     DirectIOSynchronousAnswer **synchronous_answer) {
    return writeToSocket(socket_service,
                         service_identity,
                         completeDataPack(data_pack),
                         header_deallocation_handler,
                         data_deallocation_handler,
                         synchronous_answer);
}

//send data with zmq tech
int64_t ZMQDirectIOClientConnection::writeToSocket(void *socket,
                                                   std::string& identity,
                                                   DirectIODataPack *data_pack,
                                                   DirectIOClientDeallocationHandler *header_deallocation_handler,
                                                   DirectIOClientDeallocationHandler *data_deallocation_handler,
                                                   DirectIOSynchronousAnswer **synchronous_answer) {
    assert(socket && data_pack);
    int err = 0;
    zmq_msg_t msg_data;
    zmq_msg_t msg_header_data;
    uint16_t sending_opcode = data_pack->header.dispatcher_header.fields.channel_opcode;

    ZMQDirectIOClientConnectionReadLock lock(mutex_socket_manipolation);
    //send global header
    data_pack->header.dispatcher_header.raw_data = DIRECT_IO_SET_DISPATCHER_DATA(data_pack->header.dispatcher_header.raw_data);
    data_pack->header.channel_header_size = DIRECT_IO_SET_CHANNEL_HEADER_SIZE(data_pack->header.channel_header_size);
    data_pack->header.channel_data_size = DIRECT_IO_SET_CHANNEL_DATA_SIZE(data_pack->header.channel_data_size);
    
    //send identity
    //stringSendMore(socket, identity.c_str());
    
    //send zmq envelop delimiter
    stringSendMore(socket, EmptyMessage);
    
    //check what send
    switch(data_pack->header.dispatcher_header.fields.channel_part) {
        case DIRECT_IO_CHANNEL_PART_EMPTY:
            ZMQ_DO_AGAIN(err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, _send_no_wait_flag);)
            if(err == -1) {
                err = zmq_errno();
                ZMQDIO_CONNECTION_LERR_ << "Error sending header part code:"<< zmq_strerror(err);
            }
            break;
        case DIRECT_IO_CHANNEL_PART_HEADER_ONLY:
            ZMQ_DO_AGAIN(err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, _send_more_no_wait_flag);)
            if(err == -1) {
                err = zmq_errno();
                ZMQDIO_CONNECTION_LERR_ << "Error sending header part code:"<< zmq_strerror(err);
            } else {
                //initlize message for channel custom header
                err = zmq_msg_init_data (&msg_header_data,
                                         data_pack->channel_header_data,
                                         data_pack->header.channel_header_size,
                                         DirectIOForwarder::freeSentData,
                                         new DisposeSentMemoryInfo(header_deallocation_handler,
                                                                   DisposeSentMemoryInfo::SentPartHeader,
                                                                   sending_opcode));
                if (err == -1) {
                    ZMQDIO_CONNECTION_LERR_ << "Error initializing message for message header";
                } else {
                    //channel_header_data memory not more managed by us
                    data_pack->channel_header_data = NULL;
                    //send data
                    ZMQ_DO_AGAIN(err = zmq_sendmsg(socket, &msg_header_data, _send_no_wait_flag);)
                    if(err == -1) {
                        err = zmq_errno();
                        ZMQDIO_CONNECTION_LERR_ << "Error sending message for header data part with code:"<< zmq_strerror(err);
                    } else {
                            //with the last write if there are no error we put the err to 0
                        err = 0;
                    }
                    //close the zmq message for channel custom header
                    zmq_msg_close(&msg_header_data);
                }
            }
            break;
        case DIRECT_IO_CHANNEL_PART_DATA_ONLY:
            ZMQ_DO_AGAIN(err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, _send_more_no_wait_flag);)
            if(err == -1) {
                err = zmq_errno();
                ZMQDIO_CONNECTION_LERR_ << "Error sending header part code:"<< zmq_strerror(err);
            } else {
                //initlize zmq message for channel data
                err = zmq_msg_init_data (&msg_data,
                                         data_pack->channel_data,
                                         data_pack->header.channel_data_size,
                                         DirectIOForwarder::freeSentData,
                                         new DisposeSentMemoryInfo(data_deallocation_handler, DisposeSentMemoryInfo::SentPartData, sending_opcode));
                if(err == -1) {
                    err = zmq_errno();
                    ZMQDIO_CONNECTION_LERR_ << "Error initializing message for channel data with error:"<< zmq_strerror(err);
                } else {
                    //we not manage anymore channel data
                    data_pack->channel_data = NULL;
                    
                    //send message with channel data
                    ZMQ_DO_AGAIN(err = zmq_sendmsg(socket, &msg_data, _send_no_wait_flag);)
                    if(err == -1) {
                        err = zmq_errno();
                        ZMQDIO_CONNECTION_LERR_ << "Error sending channel data with code:"<< zmq_strerror(err);
                    } else {
                        //with the last write if there are no error we put the err to 0
                        err = 0;
                    }
                }
                //close the zmq message for channel data
                zmq_msg_close(&msg_data);
            }
            break;
            
        case DIRECT_IO_CHANNEL_PART_HEADER_DATA:
            ZMQ_DO_AGAIN(err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, _send_more_no_wait_flag);)
            if(err == -1) {
                err = zmq_errno();
                ZMQDIO_CONNECTION_LERR_ << "Error sending header with code:"<< zmq_strerror(err);
            } else {
                err = zmq_msg_init_data (&msg_header_data,
                                         data_pack->channel_header_data,
                                         data_pack->header.channel_header_size,
                                         DirectIOForwarder::freeSentData,
                                         new DisposeSentMemoryInfo(header_deallocation_handler,
                                                                   DisposeSentMemoryInfo::SentPartHeader,
                                                                   sending_opcode));
                if(err == -1) {
                    err = zmq_errno();
                    ZMQDIO_CONNECTION_LERR_ << "Error initializing message for channel custom header with error:"<< zmq_strerror(err);
                } else {
                    //we not manage anymore channel custom header
                    data_pack->channel_header_data = NULL;
                    
                    //send channel custom header
                    ZMQ_DO_AGAIN(err = zmq_sendmsg(socket, &msg_header_data, _send_more_no_wait_flag);)
                    if(err == -1) {
                        err = zmq_errno();
                        ZMQDIO_CONNECTION_LERR_ << "Error sending channel custom header with code:"<< zmq_strerror(err);
                    } else {
                        //now we can send data part
                        err = zmq_msg_init_data (&msg_data,
                                                 data_pack->channel_data,
                                                 data_pack->header.channel_data_size,
                                                 DirectIOForwarder::freeSentData,
                                                 new DisposeSentMemoryInfo(data_deallocation_handler,
                                                                           DisposeSentMemoryInfo::SentPartData,
                                                                           sending_opcode));
                        if(err == -1) {
                            err = zmq_errno();
                            ZMQDIO_CONNECTION_LERR_ << "Error initializing message for channel data with error:"<< zmq_strerror(err);
                        } else {
                            //we not manage anymore the channel data
                            data_pack->channel_data = NULL;
                            
                            //send data
                            ZMQ_DO_AGAIN(err = zmq_sendmsg(socket, &msg_data, _send_no_wait_flag);)
                            if(err == -1) {
                                err = zmq_errno();
                                ZMQDIO_CONNECTION_LERR_ << "Error sending channel data with code:"<< zmq_strerror(err);
                            } else {
                                    //with the last write if there are no error we put the err to 0
                                err = 0;
                            }
                            
                            //close the zmq message for channel custom header
                            zmq_msg_close(&msg_data);
                        }
                    }
                    //close the zmq message for channel custom header
                    zmq_msg_close(&msg_header_data);
                }
            }
            break;
    }
    
    if(err >= 0 &&
       data_pack->header.dispatcher_header.fields.synchronous_answer) {
        std::string empty_delimiter;
        //receive the zmq evenlod delimiter
        err = stringReceive(socket, empty_delimiter);
        if(err != -1) {
            //DirectIOSynchronousAnswer
            zmq_msg_t msg;
            err = zmq_msg_init(&msg);
            if(err == -1) {
                ZMQDIO_CONNECTION_LERR_ << "Error initializing message for asynchronous answer";
            } else {
                err = zmq_recvmsg(socket, &msg, 0);
                if(err == -1) {
                    ZMQDIO_CONNECTION_LERR_ << "Error getting message for asynchronous answer";
                } else {
                    //we have message
                    *synchronous_answer = (DirectIOSynchronousAnswer*)calloc(sizeof(DirectIOSynchronousAnswer), 1);
                    //copy data
                    (*synchronous_answer)->answer_size = (uint32_t)zmq_msg_size(&msg);
                    if(err > 0) {
                        //if we have some data copy it  otjerwhise we have an empty pack
                        (*synchronous_answer)->answer_data = malloc((*synchronous_answer)->answer_size);
                        std::memcpy((*synchronous_answer)->answer_data , zmq_msg_data(&msg), (*synchronous_answer)->answer_size);
                    }
                }
            }
            //close received message
            err = zmq_msg_close(&msg);
            //err need to be euqal to 0 for riget things
        }
    }
    
    if(data_pack->channel_header_data != NULL) {
        //delete channel custom header
        DirectIOForwarder::freeSentData(data_pack->channel_header_data,
                                        new DisposeSentMemoryInfo(header_deallocation_handler,
                                                                  DisposeSentMemoryInfo::SentPartHeader,
                                                                  sending_opcode));
    }
    
    if(data_pack->channel_data != NULL) {
        //delete channel data
        DirectIOForwarder::freeSentData(data_pack->channel_data,
                                        new DisposeSentMemoryInfo(data_deallocation_handler,
                                                                  DisposeSentMemoryInfo::SentPartData,
                                                                  sending_opcode));
    }
    free(data_pack);
    
    //send data
    return err;
}