/*
 *	ZMQBaseClass.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include <chaos/common/global.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/direct_io/impl/ZMQBaseClass.h>

#include <string>
#include <zmq.h>

using namespace std;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io::impl;

#define ZMQDIO_BASE_LAPP_ INFO_LOG(ZMQBaseClass)
#define ZMQDIO_BASE_LDBG_ DBG_LOG(ZMQBaseClass)
#define ZMQDIO_BASE_LERR_ ERR_LOG(ZMQBaseClass)


const char * const EmptyMessage = "";
const int _send_more_no_wait_flag = ZMQ_SNDMORE;
const int _send_no_wait_flag = 0;

int ZMQBaseClass::closeSocketNoWhait (void *socket) {
    int linger = 0;
    int rc = zmq_setsockopt (socket, ZMQ_LINGER, &linger, sizeof(linger));
    if(rc != 0 || errno != ETERM) {
        return rc;
    }
    return zmq_close (socket);
}
//  Receive 0MQ string from socket and convert into C string
//  Caller must free returned string. Returns NULL if the context
//  is being terminated.
int ZMQBaseClass::stringReceive(void *socket, std::string& received_string) {
    char buffer [256];
    int size = zmq_recv (socket, buffer, 255, 0);
    if (size == -1)
        return zmq_errno();
    if (size > 255)
        size = 255;
    buffer[size] = 0;
    received_string = buffer;
    return 0;
}

//  Convert C string to 0MQ string and send to socket
int ZMQBaseClass::stringSend(void *socket, const char *string) {
    int size = zmq_send (socket, string, strlen(string), 0);
    return size;
}

//  Sends string as 0MQ string, as multipart non-terminal
int ZMQBaseClass::stringSendMore(void *socket, const char *string) {
    int err = zmq_send (socket, string, strlen(string), ZMQ_SNDMORE);
    if(err == -1) {
        err = zmq_errno();
    } else {
        err = 0;
    }
    return err;
}

int ZMQBaseClass::setID(void *socket) {
    std::string uid = UUIDUtil::generateUUIDLite();
    return zmq_setsockopt (socket, ZMQ_IDENTITY, uid.c_str(), uid.size());
}

int ZMQBaseClass::setAndReturnID(void *socket, std::string& new_id) {
    new_id = UUIDUtil::generateUUIDLite();
    return zmq_setsockopt (socket, ZMQ_IDENTITY, new_id.c_str(), new_id.size());
}

int ZMQBaseClass::sendStartEnvelop(void *socket) {
    //sending envelop delimiter
    return stringSendMore(socket, EmptyMessage);
}

int ZMQBaseClass::receiveStartEnvelop(void *socket) {
    std::string empty_delimiter;
    return  stringReceive(socket, empty_delimiter);
}

#define C_DIO_GET_HANDLE_POINTER(X) (*X)

int ZMQBaseClass::reveiceDatapack(void *socket,
                                  std::string& identity,
                                  DirectIODataPack **data_pack_handle) {
    int                     err = 0;
    std::string             empty_delimiter;
    char					header_buffer[DIRECT_IO_HEADER_SIZE];
    
    if((err = stringReceive(socket, identity)) == -1) {
        return zmq_errno();
    }
    
    //receive the zmq evenlop delimiter
    if((err = stringReceive(socket, empty_delimiter)) == -1) {
        return zmq_errno();
    }
    
    //read header
    err = zmq_recv(socket, header_buffer, DIRECT_IO_HEADER_SIZE, 0);
    if(err == -1) {
        return zmq_errno();
    }
    
    if(err != DIRECT_IO_HEADER_SIZE) {
        return -1;
    }
    
    //create new datapack
    *data_pack_handle = new DirectIODataPack();
    
    //clear all memory
    std::memset(C_DIO_GET_HANDLE_POINTER(data_pack_handle), 0, sizeof(DirectIODataPack));
    
    //set dispatch header data
    C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.dispatcher_header.raw_data = DIRECT_IO_GET_DISPATCHER_DATA(header_buffer);
    
    //check what i need to reice
    switch(C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.dispatcher_header.fields.channel_part) {
        case DIRECT_IO_CHANNEL_PART_EMPTY:
            break;
        case DIRECT_IO_CHANNEL_PART_HEADER_ONLY:
            //init header data buffer
            C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_header_size = DIRECT_IO_GET_CHANNEL_HEADER_SIZE(header_buffer);
            C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_header_data = malloc(C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_header_size);
            C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_data = NULL;
            
            //init message with buffer
            err = zmq_recv(socket,
                           C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_header_data,
                           C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_header_size,
                           0);
            //err = zmq_msg_recv(&m_header_data, socket, 0);
            if(err == -1) {
                err = zmq_errno();
                free(C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_header_data);
                delete C_DIO_GET_HANDLE_POINTER(data_pack_handle);
            } else {
                //if all goes weel we need to have 0 error an no the byte received
                err = 0;
            }
            break;
        case DIRECT_IO_CHANNEL_PART_DATA_ONLY:
            //init data buffer
            C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_data_size = DIRECT_IO_GET_CHANNEL_DATA_SIZE(header_buffer);
            C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_data = malloc(C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_data_size);
            C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_header_data = NULL;
            
            //init message with buffer
            err = zmq_recv(socket,
                           C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_data,
                           C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_data_size,
                           0);
            if(err == -1) {
                err = zmq_errno();
                free(C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_data);
                delete C_DIO_GET_HANDLE_POINTER(data_pack_handle);
            } else {
                //if all goes weel we need to have 0 error an no the byte received
                err = 0;
            }
            break;
        case DIRECT_IO_CHANNEL_PART_HEADER_DATA:
            //init header data and channel data buffers
            C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_header_size = DIRECT_IO_GET_CHANNEL_HEADER_SIZE(header_buffer);
            C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_header_data = malloc(C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_header_size);
            
            C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_data_size = DIRECT_IO_GET_CHANNEL_DATA_SIZE(header_buffer);
            C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_data = malloc(C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_data_size);
            
            //reiceve all data
            err = zmq_recv(socket,
                           C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_header_data,
                           C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_header_size,
                           0);
            if(err == -1) {
                err = zmq_errno();
                free(C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_header_data);
                free(C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_data);
                delete C_DIO_GET_HANDLE_POINTER(data_pack_handle);
            }
            
            err = zmq_recv(socket,
                           C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_data,
                           C_DIO_GET_HANDLE_POINTER(data_pack_handle)->header.channel_data_size,
                           0);
            if(err == -1) {
                err = zmq_errno();
                free(C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_header_data);
                free(C_DIO_GET_HANDLE_POINTER(data_pack_handle)->channel_data);
                delete C_DIO_GET_HANDLE_POINTER(data_pack_handle);
            } else {
                //if all goes weel we need to have 0 error an no the byte received
                err = 0;
            }
            break;
    }
    return err;
}

int ZMQBaseClass::sendDatapack(void *socket,
                               std::string identity,
                               DirectIODataPack *data_pack,
                               DirectIOClientDeallocationHandler *header_deallocation_handler,
                               DirectIOClientDeallocationHandler *data_deallocation_handler) {
    int err = 0;
    zmq_msg_t msg_data;
    zmq_msg_t msg_header_data;
    uint16_t sending_opcode = data_pack->header.dispatcher_header.fields.channel_opcode;
    
    //send global header
    data_pack->header.dispatcher_header.raw_data = DIRECT_IO_SET_DISPATCHER_DATA(data_pack->header.dispatcher_header.raw_data);
    data_pack->header.channel_header_size = DIRECT_IO_SET_CHANNEL_HEADER_SIZE(data_pack->header.channel_header_size);
    data_pack->header.channel_data_size = DIRECT_IO_SET_CHANNEL_DATA_SIZE(data_pack->header.channel_data_size);
    
    //send identity
    //stringSendMore(socket, identity.c_str());
    
    //send zmq envelop delimiter
    //stringSendMore(socket, EmptyMessage);
    sendStartEnvelop(socket);
    
    //check what send
    switch(data_pack->header.dispatcher_header.fields.channel_part) {
        case DIRECT_IO_CHANNEL_PART_EMPTY:
            ZMQ_DO_AGAIN(err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, _send_no_wait_flag);)
            if(err == -1) {
                err = zmq_errno();
                ZMQDIO_BASE_LERR_ << "Error sending header part code:"<< zmq_strerror(err);
            }
            break;
        case DIRECT_IO_CHANNEL_PART_HEADER_ONLY:
            ZMQ_DO_AGAIN(err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, _send_more_no_wait_flag);)
            if(err == -1) {
                err = zmq_errno();
                ZMQDIO_BASE_LERR_ << "Error sending header part code:"<< zmq_strerror(err);
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
                    ZMQDIO_BASE_LERR_ << "Error initializing message for message header";
                } else {
                    //channel_header_data memory not more managed by us
                    data_pack->channel_header_data = NULL;
                    //send data
                    ZMQ_DO_AGAIN(err = zmq_sendmsg(socket, &msg_header_data, _send_no_wait_flag);)
                    if(err == -1) {
                        err = zmq_errno();
                        ZMQDIO_BASE_LERR_ << "Error sending message for header data part with code:"<< zmq_strerror(err);
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
                ZMQDIO_BASE_LERR_ << "Error sending header part code:"<< zmq_strerror(err);
            } else {
                //initlize zmq message for channel data
                err = zmq_msg_init_data (&msg_data,
                                         data_pack->channel_data,
                                         data_pack->header.channel_data_size,
                                         DirectIOForwarder::freeSentData,
                                         new DisposeSentMemoryInfo(data_deallocation_handler, DisposeSentMemoryInfo::SentPartData, sending_opcode));
                if(err == -1) {
                    err = zmq_errno();
                    ZMQDIO_BASE_LERR_ << "Error initializing message for channel data with error:"<< zmq_strerror(err);
                } else {
                    //we not manage anymore channel data
                    data_pack->channel_data = NULL;
                    
                    //send message with channel data
                    ZMQ_DO_AGAIN(err = zmq_sendmsg(socket, &msg_data, _send_no_wait_flag);)
                    if(err == -1) {
                        err = zmq_errno();
                        ZMQDIO_BASE_LERR_ << "Error sending channel data with code:"<< zmq_strerror(err);
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
                ZMQDIO_BASE_LERR_ << "Error sending header with code:"<< zmq_strerror(err);
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
                    ZMQDIO_BASE_LERR_ << "Error initializing message for channel custom header with error:"<< zmq_strerror(err);
                } else {
                    //we not manage anymore channel custom header
                    data_pack->channel_header_data = NULL;
                    
                    //send channel custom header
                    ZMQ_DO_AGAIN(err = zmq_sendmsg(socket, &msg_header_data, _send_more_no_wait_flag);)
                    if(err == -1) {
                        err = zmq_errno();
                        ZMQDIO_BASE_LERR_ << "Error sending channel custom header with code:"<< zmq_strerror(err);
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
                            ZMQDIO_BASE_LERR_ << "Error initializing message for channel data with error:"<< zmq_strerror(err);
                        } else {
                            //we not manage anymore the channel data
                            data_pack->channel_data = NULL;
                            
                            //send data
                            ZMQ_DO_AGAIN(err = zmq_sendmsg(socket, &msg_data, _send_no_wait_flag);)
                            if(err == -1) {
                                err = zmq_errno();
                                ZMQDIO_BASE_LERR_ << "Error sending channel data with code:"<< zmq_strerror(err);
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
    return err;
}