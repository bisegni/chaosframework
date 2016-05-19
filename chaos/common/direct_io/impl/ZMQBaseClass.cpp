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

#include <boost/format.hpp>

using namespace std;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io::impl;

#define ZMQDIO_BASE_LAPP_ INFO_LOG(ZMQBaseClass)
#define ZMQDIO_BASE_LDBG_ DBG_LOG(ZMQBaseClass)
#define ZMQDIO_BASE_LERR_ ERR_LOG(ZMQBaseClass)

#define EXIT_IF_NO_MORE_MESSAGE(err_to_report, message) \
if((err = moreMessageToRead(socket, have_more_message))){ \
ZMQDIO_BASE_LERR_<< "Error checking other message in socket"; \
return err; \
} else if(have_more_message == false){ \
ZMQDIO_BASE_LERR_<< message;\
return err;\
}

#define SYNC_DELETE_HEADER_AND_DATA(mem,dealloc,part,opcode)\
DirectIOForwarder::freeSentData(mem,\
new DisposeSentMemoryInfo(dealloc,\
part,\
opcode));\
mem = NULL;

#define PRINT_ZMQ_ERR(err)\
boost::str(boost::format("zmq error %1% [%2%]")%err%zmq_strerror(err))

const char * const EmptyMessage = "";

int ZMQBaseClass::closeSocketNoWhait (void *socket) {
    int linger = 0;
    int rc = zmq_setsockopt (socket, ZMQ_LINGER, &linger, sizeof(linger));
    if(rc != 0 || errno != ETERM) {
        return rc;
    }
    return zmq_close (socket);
}

int ZMQBaseClass::readMessage(void * socket,
                              void *message_data,
                              size_t message_max_size,
                              size_t& message_size_read) {
    int err = 0;
    //no we can read the message
    if((err = zmq_recv(socket,
                       message_data,
                       message_max_size,
                       0)) == -1) {
        //we got an error
        err = zmq_errno();
        ZMQDIO_BASE_LERR_ << "Error receiving data from socket with code:" << PRINT_ZMQ_ERR(err);
        return err;
    } else {
        //take the readed byte
        message_size_read = err;
    }
    //return success
    return 0;
}

//! send a new message from zmq socket
/*!
 
 */
int ZMQBaseClass::sendMessage(void *socket,
                              void **message_data,
                              size_t message_size,
                              zmq_free_fn *ffn,
                              void *hint,
                              bool more_to_send) {
    int err = 0;
    zmq_msg_t message;
    
    if((err = zmq_msg_init_data(&message,
                                *message_data,
                                message_size,
                                ffn,
                                hint)) == -1){
        //error creating the message
        err = zmq_errno();
        ZMQDIO_BASE_LERR_ << "Error initializing message with error:" << PRINT_ZMQ_ERR(err);
    } else {
        //message can be forward so we reset the usse pointer
        *message_data = NULL;
        //send data
        if((err = zmq_sendmsg(socket, &message, more_to_send?ZMQ_SNDMORE:ZMQ_DONTWAIT)) == -1){
            err = zmq_errno();
            ZMQDIO_BASE_LERR_ << "Error sending message with error:" << PRINT_ZMQ_ERR(err);
        } else {
            //reset the error
            err = 0;
        }
        //close the message
        zmq_msg_close(&message);
    }
    return err;
}

int ZMQBaseClass::sendMessage(void *socket,
                              void *message_data,
                              size_t message_size,
                              bool more_to_send) {
    int err = 0;
    zmq_msg_t message;
    
    if((err = zmq_msg_init_size(&message,
                                message_size)) == -1){
        //error creating the message
        err = zmq_errno();
        ZMQDIO_BASE_LERR_ << "Error initializing message with error:" << PRINT_ZMQ_ERR(err);
    } else {
        //copy content into message
        memcpy(zmq_msg_data(&message),
               message_data,
               message_size);
        //send data
        if((err = zmq_sendmsg(socket, &message, more_to_send?ZMQ_SNDMORE:ZMQ_DONTWAIT)) == -1){
            err = zmq_errno();
            ZMQDIO_BASE_LERR_ << "Error sending message with error:" << PRINT_ZMQ_ERR(err);
        } else {
            //reset the error
            err = 0;
        }
        //close the message
        zmq_msg_close(&message);
    }
    return err;
}

int ZMQBaseClass::moreMessageToRead(void * socket,
                                    bool& more_to_read) {
    int err = 0;
    int option_result = 0;
    size_t size_int = sizeof(int);
    
    //we heva received the message now check the size aspected
    if((err = zmq_getsockopt(socket, ZMQ_RCVMORE, &option_result, &size_int)) == -1) {
        ZMQDIO_BASE_LERR_ << "Error checking the send more option on socket with error:" << PRINT_ZMQ_ERR(err);
        err = zmq_errno();
        return err;
    } else {
        more_to_read = (bool)option_result;
    }
    return 0;
}

//  Receive 0MQ string from socket and convert into C string
//  Caller must free returned string. Returns NULL if the context
//  is being terminated.
int ZMQBaseClass::stringReceive(void *socket, std::string& received_string) {
    char buffer [256];
    size_t readed_byte = 0;
    
    //read message and check the error
    int err = readMessage(socket, buffer, 255, readed_byte);
    if(err) return err;
    
    //we got string so cap it for nullify at the end
    buffer[readed_byte] = 0;
    //return the string
    received_string = buffer;
    //return success
    return 0;
}

//  Convert C string to 0MQ string and send to socket
int ZMQBaseClass::stringSend(void *socket, const char *string) {
    return sendMessage(socket, (void*)string, strlen(string), false);
}

//  Sends string as 0MQ string, as multipart non-terminal
int ZMQBaseClass::stringSendMore(void *socket, const char *string) {
    return sendMessage(socket, (void*)string, strlen(string), true);
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
    int err = 0;
    if((err = stringReceive(socket, empty_delimiter))){
        return err;
    }
    //assert on different delimiter size
    CHAOS_ASSERT(empty_delimiter.size() == 0)
    return err;
}

int ZMQBaseClass::reveiceDatapack(void *socket,
                                  std::string& identity,
                                  DirectIODataPack **data_pack_handle) {
    //read first the identity
    int err = 0;
    if((err = stringReceive(socket, identity))) {
        return err;
    }
    
    //check the identity size that need to be different from zero
    if(identity.size()==0){
        ZMQDIO_BASE_LERR_<<" malformed packet, empty identity";
        return -12000;
    }
    
    //read the direct io datapack on zmq messages
    return reveiceDatapack(socket,
                           data_pack_handle);
}

int ZMQBaseClass::reveiceDatapack(void *socket,
                                  DirectIODataPack **data_pack_handle) {
    int                     err = 0;
    std::string             empty_delimiter;
    bool                    have_more_message = false;
    size_t                  readed_byte;
    char					header_buffer[DIRECT_IO_HEADER_SIZE];
    DirectIODataPack        *data_pack_ptr = NULL;
    //receive the zmq evenlop delimiter
    if((err = receiveStartEnvelop(socket))) {
        return err;
    }
    
    //check if we have other message
    EXIT_IF_NO_MORE_MESSAGE(-13000, "No other message after envelop");
    
    //read header
    if((err = readMessage(socket, header_buffer, DIRECT_IO_HEADER_SIZE, readed_byte))){
        return err;
    }
    
    if(DIRECT_IO_HEADER_SIZE != readed_byte) {
        ZMQDIO_BASE_LERR_<< "The header read phase has reported a different size of '"<<readed_byte<<"' bytes"; \
        return -13001;
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
            
            //check if we have header message
            EXIT_IF_NO_MORE_MESSAGE(-13002, "No other message after header for DIRECT_IO_CHANNEL_PART_HEADER_ONLY");
            
            //init header data buffer
            data_pack_ptr->header.channel_header_size = DIRECT_IO_GET_CHANNEL_HEADER_SIZE(header_buffer);
            data_pack_ptr->channel_header_data = malloc(data_pack_ptr->header.channel_header_size);
            data_pack_ptr->channel_data = NULL;
            
            //read the channel header
            if((err = readMessage(socket,
                                  data_pack_ptr->channel_header_data,
                                  data_pack_ptr->header.channel_header_size,
                                  readed_byte))){
                ZMQDIO_BASE_LERR_<< "Error reading the channel header with code:"<<err;
                //free header memory
                free(data_pack_ptr->channel_header_data);
                //free datapack memory
                free(data_pack_ptr);
            }
            break;
        case DIRECT_IO_CHANNEL_PART_DATA_ONLY:
            
            //check if we have data message
            EXIT_IF_NO_MORE_MESSAGE(-13003, "No other message after header for DIRECT_IO_CHANNEL_PART_DATA_ONLY");
            
            //init data buffer
            data_pack_ptr->header.channel_data_size = DIRECT_IO_GET_CHANNEL_DATA_SIZE(header_buffer);
            data_pack_ptr->channel_data = malloc(data_pack_ptr->header.channel_data_size);
            data_pack_ptr->channel_header_data = NULL;
            
            //read data part
            if((err = readMessage(socket,
                                  data_pack_ptr->channel_data,
                                  data_pack_ptr->header.channel_data_size,
                                  readed_byte))){
                ZMQDIO_BASE_LERR_<< "Error reading the channel header with code:"<<err;
                //free data memory
                free(data_pack_ptr->channel_data);
                //free datapack memory
                free(data_pack_ptr);
            }
            break;
        case DIRECT_IO_CHANNEL_PART_HEADER_DATA:
            //check if we have header message
            EXIT_IF_NO_MORE_MESSAGE(-13004, "No other message after header for DIRECT_IO_CHANNEL_PART_HEADER_DATA");
            
            //allocate header buffer
            data_pack_ptr->header.channel_header_size = DIRECT_IO_GET_CHANNEL_HEADER_SIZE(header_buffer);
            data_pack_ptr->channel_header_data = malloc(data_pack_ptr->header.channel_header_size);
            
            if((err = readMessage(socket,
                                  data_pack_ptr->channel_header_data,
                                  data_pack_ptr->header.channel_header_size,
                                  readed_byte))){
                ZMQDIO_BASE_LERR_<< "Error reading the channel header with code:"<<err;
                //free header memory
                free(data_pack_ptr->channel_header_data);
                //free datapack memory
                free(data_pack_ptr);
            } else {
                //check if we have data message
                EXIT_IF_NO_MORE_MESSAGE(-13005, "No other message after header for DIRECT_IO_CHANNEL_PART_HEADER_DATA");
                
                //allocate data buffer
                data_pack_ptr->header.channel_data_size = DIRECT_IO_GET_CHANNEL_DATA_SIZE(header_buffer);
                data_pack_ptr->channel_data = malloc(data_pack_ptr->header.channel_data_size);
                
                //read data part
                if((err = readMessage(socket,
                                      data_pack_ptr->channel_data,
                                      data_pack_ptr->header.channel_data_size,
                                      readed_byte))){
                    ZMQDIO_BASE_LERR_<< "Error reading the channel header with code:"<<err;
                    //free header memory
                    free(data_pack_ptr->channel_header_data);
                    //free data memory
                    free(data_pack_ptr->channel_data);
                    //free datapack memory
                    free(data_pack_ptr);
                }
            }
            break;
    }
    return err;
}

int ZMQBaseClass::sendDatapack(void *socket,
                               std::string identity,
                               DirectIODataPack *data_pack,
                               DirectIODeallocationHandler *header_deallocation_handler,
                               DirectIODeallocationHandler *data_deallocation_handler) {
    //send identity
    int err = 0;
    if((err = stringSendMore(socket, identity.c_str()))) {
        return err;
    }
    //read the direct io datapack with zmq messages
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
    uint16_t sending_opcode = data_pack->header.dispatcher_header.fields.channel_opcode;
    
    //send global header
    data_pack->header.dispatcher_header.raw_data = DIRECT_IO_SET_DISPATCHER_DATA(data_pack->header.dispatcher_header.raw_data);
    data_pack->header.channel_header_size = DIRECT_IO_SET_CHANNEL_HEADER_SIZE(data_pack->header.channel_header_size);
    data_pack->header.channel_data_size = DIRECT_IO_SET_CHANNEL_DATA_SIZE(data_pack->header.channel_data_size);
    
    //send first the direct io envelop delimiter
    if((err = sendStartEnvelop(socket))) {
        return err;
    } else {
        //envelope has been sent so we can send direct io messages
        switch(data_pack->header.dispatcher_header.fields.channel_part) {
            case DIRECT_IO_CHANNEL_PART_EMPTY:
                if((err = sendMessage(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, false))) {
                    ZMQDIO_BASE_LERR_ << "Error sending header part:"<< PRINT_ZMQ_ERR(err);
                }
                break;
                
            case DIRECT_IO_CHANNEL_PART_HEADER_ONLY:
                if((err = sendMessage(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, true))) {
                    ZMQDIO_BASE_LERR_ << "Error sending header part:"<< PRINT_ZMQ_ERR(err);
                } else if((err = sendMessage(socket,
                                             &data_pack->channel_header_data,
                                             (size_t)data_pack->header.channel_header_size,
                                             DirectIOForwarder::freeSentData,
                                             new DisposeSentMemoryInfo(header_deallocation_handler,
                                                                       DisposeSentMemoryInfo::SentPartHeader,
                                                                       sending_opcode),
                                             false))){
                    ZMQDIO_BASE_LERR_ << "Error sending channel header part:"<< PRINT_ZMQ_ERR(err);
                }
                break;
                
            case DIRECT_IO_CHANNEL_PART_DATA_ONLY:
                if((err = sendMessage(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, true))) {
                    ZMQDIO_BASE_LERR_ << "Error sending header part:"<< PRINT_ZMQ_ERR(err);
                } else if((err = sendMessage(socket,
                                             &data_pack->channel_data,
                                             (size_t)data_pack->header.channel_data_size,
                                             DirectIOForwarder::freeSentData,
                                             new DisposeSentMemoryInfo(data_deallocation_handler,
                                                                       DisposeSentMemoryInfo::SentPartData,
                                                                       sending_opcode),
                                             false))){
                    ZMQDIO_BASE_LERR_ << "Error sending channel data part:"<< PRINT_ZMQ_ERR(err);
                }
                break;
                
            case DIRECT_IO_CHANNEL_PART_HEADER_DATA:
                if((err = sendMessage(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, true))) {
                    ZMQDIO_BASE_LERR_ << "Error sending header part:"<< PRINT_ZMQ_ERR(err);
                } else if((err = sendMessage(socket,
                                             &data_pack->channel_header_data,
                                             (size_t)data_pack->header.channel_header_size,
                                             DirectIOForwarder::freeSentData,
                                             new DisposeSentMemoryInfo(header_deallocation_handler,
                                                                       DisposeSentMemoryInfo::SentPartHeader,
                                                                       sending_opcode),
                                             true))){
                    ZMQDIO_BASE_LERR_ << "Error sending channel header part:"<< PRINT_ZMQ_ERR(err);
                    //error sending header data
                } else if((err = sendMessage(socket,
                                             &data_pack->channel_data,
                                             (size_t)data_pack->header.channel_data_size,
                                             DirectIOForwarder::freeSentData,
                                             new DisposeSentMemoryInfo(data_deallocation_handler,
                                                                       DisposeSentMemoryInfo::SentPartData,
                                                                       sending_opcode),
                                             false))){
                    ZMQDIO_BASE_LERR_ << "Error sending channel data part:"<< PRINT_ZMQ_ERR(err);
                    if(data_pack->channel_data != NULL) {
                        ZMQDIO_BASE_LERR_ << "Free the channel data memory";
                        SYNC_DELETE_HEADER_AND_DATA(data_pack->channel_data,
                                                    data_deallocation_handler,
                                                    DisposeSentMemoryInfo::SentPartData,
                                                    sending_opcode);
                    }
                }
                break;
        }
    }
    
    //clean header or data part in case of error
    if(data_pack->channel_header_data != NULL) {
        ZMQDIO_BASE_LERR_ << "Free channel header memory";
        SYNC_DELETE_HEADER_AND_DATA(data_pack->channel_header_data,
                                    header_deallocation_handler,
                                    DisposeSentMemoryInfo::SentPartHeader,
                                    sending_opcode);
    }
    
    if(data_pack->channel_data != NULL) {
        ZMQDIO_BASE_LERR_ << "Free the channel data memory";
        SYNC_DELETE_HEADER_AND_DATA(data_pack->channel_data,
                                    data_deallocation_handler,
                                    DisposeSentMemoryInfo::SentPartData,
                                    sending_opcode);
    }
    
    return err;
}