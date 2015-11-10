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

int ZMQBaseClass::reveiceDatapack(void *socket,
                                  std::string& identity,
                                  DirectIODataPack **data_pack_handle) {
    if(stringReceive(socket, identity) == -1) {
        return zmq_errno();
    }
    if(identity.size()==0){
        ZMQDIO_BASE_LERR_<<" malformed packet, empty identity";
        return -12000;
    }
    return reveiceDatapack(socket,
                           data_pack_handle);
}

int ZMQBaseClass::reveiceDatapack(void *socket,
                                  DirectIODataPack **data_pack_handle) {
    int                     err = 0;
    std::string             empty_delimiter;
    char					header_buffer[DIRECT_IO_HEADER_SIZE];
    DirectIODataPack        *data_pack_ptr = NULL;
    //receive the zmq evenlop delimiter
    if((err = receiveStartEnvelop(socket))) {
        return err;
    }
    
    //read header
    if((err = zmq_recv(socket,
                       header_buffer,
                       DIRECT_IO_HEADER_SIZE, 0)) == -1) {
        return zmq_errno();
    }
    
    //check if we have received the rigth header number of bytes
    if(err != DIRECT_IO_HEADER_SIZE) {
        return -1;
    } else {
        err = 0;
    }
    
    //create new datapack
    data_pack_ptr = *data_pack_handle = (DirectIODataPack*)calloc(1, sizeof(DirectIODataPack));
    
    //clear all memory
    std::memset(data_pack_ptr, 0, sizeof(DirectIODataPack));
    
    //set dispatch header data
    data_pack_ptr->header.dispatcher_header.raw_data = DIRECT_IO_GET_DISPATCHER_DATA(header_buffer);
    
    //check what i need to reice
    switch(data_pack_ptr->header.dispatcher_header.fields.channel_part) {
        case DIRECT_IO_CHANNEL_PART_EMPTY:
            break;
        case DIRECT_IO_CHANNEL_PART_HEADER_ONLY:
            //init header data buffer
            data_pack_ptr->header.channel_header_size = DIRECT_IO_GET_CHANNEL_HEADER_SIZE(header_buffer);
            data_pack_ptr->channel_header_data = malloc(data_pack_ptr->header.channel_header_size);
            data_pack_ptr->channel_data = NULL;
            
            //init message with buffer
            err = zmq_recv(socket,
                           data_pack_ptr->channel_header_data,
                           data_pack_ptr->header.channel_header_size,
                           0);
            //err = zmq_msg_recv(&m_header_data, socket, 0);
            if(err == -1) {
                err = zmq_errno();
                free(data_pack_ptr->channel_header_data);
                free(data_pack_ptr);
            } else {
                //if all goes weel we need to have 0 error an no the byte received
                err = 0;
            }
            break;
        case DIRECT_IO_CHANNEL_PART_DATA_ONLY:
            //init data buffer
            data_pack_ptr->header.channel_data_size = DIRECT_IO_GET_CHANNEL_DATA_SIZE(header_buffer);
            data_pack_ptr->channel_data = malloc(data_pack_ptr->header.channel_data_size);
            data_pack_ptr->channel_header_data = NULL;
            
            //init message with buffer
            err = zmq_recv(socket,
                           data_pack_ptr->channel_data,
                           data_pack_ptr->header.channel_data_size,
                           0);
            if(err == -1) {
                err = zmq_errno();
                free(data_pack_ptr->channel_data);
                free(data_pack_ptr);
            } else {
                //if all goes weel we need to have 0 error an no the byte received
                err = 0;
            }
            break;
        case DIRECT_IO_CHANNEL_PART_HEADER_DATA:
            //allocate header buffer
            data_pack_ptr->header.channel_header_size = DIRECT_IO_GET_CHANNEL_HEADER_SIZE(header_buffer);
            data_pack_ptr->channel_header_data = malloc(data_pack_ptr->header.channel_header_size);
            
            //receive the header
            err = zmq_recv(socket,
                           data_pack_ptr->channel_header_data,
                           data_pack_ptr->header.channel_header_size,
                           0);
            if(err == -1) {
                //error reading buffer so we need to delete header memory and datapack
                err = zmq_errno();
                free(data_pack_ptr->channel_header_data);
                free(data_pack_ptr);
            } else {
                //allocate data buffer
                data_pack_ptr->header.channel_data_size = DIRECT_IO_GET_CHANNEL_DATA_SIZE(header_buffer);
                data_pack_ptr->channel_data = malloc(data_pack_ptr->header.channel_data_size);
                //receive the data
                err = zmq_recv(socket,
                               data_pack_ptr->channel_data,
                               data_pack_ptr->header.channel_data_size,
                               0);
                if(err == -1) {
                    //error reading data so we need to delete previous readed header, data memory and datapack
                    err = zmq_errno();
                    free(data_pack_ptr->channel_header_data);
                    free(data_pack_ptr->channel_data);
                    free(data_pack_ptr);
                } else {
                    //if all goes weel we need to have 0 error an no the byte received
                    err = 0;
                }
            }
            break;
    }
    return err;
}

#define SYNC_DELETE_HEADER_AND_DATA(mem,dealloc,part,opcode)\
DirectIOForwarder::freeSentData(mem,\
new DisposeSentMemoryInfo(dealloc,\
part,\
opcode));

int ZMQBaseClass::sendDatapack(void *socket,
                               std::string identity,
                               DirectIODataPack *data_pack,
                               DirectIODeallocationHandler *header_deallocation_handler,
                               DirectIODeallocationHandler *data_deallocation_handler) {
    //send identity
    if(stringSendMore(socket, identity.c_str()) == -1) {
        return zmq_errno();
    }
    
    return sendDatapack(socket,
                        data_pack,
                        header_deallocation_handler,
                        data_deallocation_handler);
}

int ZMQBaseClass::sendDatapack(void *socket,
                               DirectIODataPack *data_pack,
                               DirectIODeallocationHandler *header_deallocation_handler,
                               DirectIODeallocationHandler *data_deallocation_handler) {
    int err = 0;
    zmq_msg_t msg_data;
    zmq_msg_t msg_header_data;
    uint16_t sending_opcode = data_pack->header.dispatcher_header.fields.channel_opcode;
    
    //send global header
    data_pack->header.dispatcher_header.raw_data = DIRECT_IO_SET_DISPATCHER_DATA(data_pack->header.dispatcher_header.raw_data);
    data_pack->header.channel_header_size = DIRECT_IO_SET_CHANNEL_HEADER_SIZE(data_pack->header.channel_header_size);
    data_pack->header.channel_data_size = DIRECT_IO_SET_CHANNEL_DATA_SIZE(data_pack->header.channel_data_size);
    
    if((err = sendStartEnvelop(socket)) == -1) {
        return zmq_errno();
    }
    
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
                    err = zmq_errno();
                    SYNC_DELETE_HEADER_AND_DATA(data_pack->channel_header_data,
                                                header_deallocation_handler,
                                                DisposeSentMemoryInfo::SentPartHeader,
                                                sending_opcode);
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
                                         new DisposeSentMemoryInfo(data_deallocation_handler,
                                                                   DisposeSentMemoryInfo::SentPartData,
                                                                   sending_opcode));
                if(err == -1) {
                    //delete data
                    err = zmq_errno();
                    SYNC_DELETE_HEADER_AND_DATA(data_pack->channel_data,
                                                data_deallocation_handler,
                                                DisposeSentMemoryInfo::SentPartData,
                                                sending_opcode);
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
                    SYNC_DELETE_HEADER_AND_DATA(data_pack->channel_header_data,
                                                header_deallocation_handler,
                                                DisposeSentMemoryInfo::SentPartHeader,
                                                sending_opcode);
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
                            SYNC_DELETE_HEADER_AND_DATA(data_pack->channel_data,
                                                        data_deallocation_handler,
                                                        DisposeSentMemoryInfo::SentPartData,
                                                        sending_opcode);
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