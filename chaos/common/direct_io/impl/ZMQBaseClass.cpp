/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/common/global.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/direct_io/impl/ZMQBaseClass.h>

#include <boost/format.hpp>

using namespace std;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::direct_io;
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

#define MESSAGE_GOOD_CONTEXT(sc) "ZMQ " #sc " for %1% configured with %2%=%3%"
#define MESSAGE_ERR_CONTEXT(sc) "Error setting ZMQ " #sc "for %1% with %2%=%3%"

#define SUCCESS_STRING_ON_CONTEXT(x) CHAOS_FORMAT(MESSAGE_GOOD_CONTEXT(Context),%domain%x%prop_value)
#define SUCCESS_STRING_ON_SOCKET(x) CHAOS_FORMAT(MESSAGE_GOOD_CONTEXT(Socket),%domain%x%prop_value)

#define ERROR_STRING_ON_CONTEXT(x) CHAOS_FORMAT(MESSAGE_ERR_CONTEXT(Context),%domain%x%prop_value)
#define ERROR_STRING_ON_SOCKET(x) CHAOS_FORMAT(MESSAGE_ERR_CONTEXT(Socket),%domain%x%prop_value)

#define SET_CONTEXT_OPTION(x,s) \
if(hasZMQProperty(default_conf,\
startup_conf,\
s,\
prop_value)) {\
err = zmq_ctx_set(context, x, prop_value);\
if(err) {\
ZMQDIO_BASE_LERR_ << ERROR_STRING_ON_CONTEXT(s);\
return err;\
} else {\
ZMQDIO_BASE_LAPP_ << SUCCESS_STRING_ON_CONTEXT(s);\
}\
return 0;\
}

#define SET_SOCKET_OPTION(s, x, p)\
if((err = setSocketOption(s, default_conf, startup_conf, x, p, domain))){return err;}


static void zqmFreeSentData(void *data,
                            void *hint) {
    ChaosUniquePtr<DisposeSentMemoryInfo> free_info(static_cast<DisposeSentMemoryInfo*>(hint));
}

bool ZMQBaseClass::hasZMQProperty(MapZMQConfiguration &default_conf,
                                  const MapZMQConfiguration &startup_conf,
                                  const std::string& prop_name,
                                  int& prop_value) {
    MapZMQConfigurationConstIterator it_def_conf = default_conf.find(prop_name);
    MapZMQConfigurationConstIterator it_str_conf = startup_conf.find(prop_name);
    
    bool def_conf_has =  it_def_conf != default_conf.end();
    bool startup_conf_has =  it_str_conf != startup_conf.end();
    
    if(startup_conf_has) {
        prop_value = boost::lexical_cast<int>(it_str_conf->second);
    } else if(def_conf_has){
        prop_value = boost::lexical_cast<int>(it_def_conf->second);
    }
    
    return (def_conf_has || startup_conf_has);
}

int ZMQBaseClass::setSocketOption(void *socket,
                                  MapZMQConfiguration &default_conf,
                                  const MapZMQConfiguration &startup_conf,
                                  int socket_option,
                                  const std::string& socket_option_name,
                                  const std::string& domain) {
    int err = 0;
    int prop_value = 0;
    if(hasZMQProperty(default_conf,
                      startup_conf,
                      socket_option_name,
                      prop_value)) {
        err = zmq_setsockopt(socket, socket_option, &prop_value, sizeof(int));
        if(err) {
            ZMQDIO_BASE_LERR_ << ERROR_STRING_ON_SOCKET(socket_option_name);
            return err;
        } else {
            ZMQDIO_BASE_LAPP_ << SUCCESS_STRING_ON_SOCKET(socket_option_name);
        }
    }
    return 0;
}

int ZMQBaseClass::configureContextWithStartupParameter(void *context,
                                                       ::MapZMQConfiguration &default_conf,
                                                       const ::MapZMQConfiguration &startup_conf,
                                                       const std::string& domain) {
    int err = 0;
    int prop_value = 0;
    SET_CONTEXT_OPTION(ZMQ_IO_THREADS,"ZMQ_IO_THREADS");
    SET_CONTEXT_OPTION(ZMQ_MAX_SOCKETS,"ZMQ_MAX_SOCKETS");
    
    return 0;
}

int ZMQBaseClass::configureSocketWithStartupParameter(void *socket,
                                                      ::MapZMQConfiguration &default_conf,
                                                      const ::MapZMQConfiguration &startup_conf,
                                                      const std::string& domain) {
    
    int err = 0;
    SET_SOCKET_OPTION(socket, ZMQ_LINGER,"ZMQ_LINGER");
    SET_SOCKET_OPTION(socket, ZMQ_RCVHWM,"ZMQ_RCVHWM");
    SET_SOCKET_OPTION(socket, ZMQ_SNDHWM,"ZMQ_SNDHWM");
    SET_SOCKET_OPTION(socket, ZMQ_RCVTIMEO,"ZMQ_RCVTIMEO");
    SET_SOCKET_OPTION(socket, ZMQ_SNDTIMEO,"ZMQ_SNDTIMEO");
    SET_SOCKET_OPTION(socket, ZMQ_SNDBUF,"ZMQ_SNDBUF");
    SET_SOCKET_OPTION(socket, ZMQ_RCVBUF,"ZMQ_RCVBUF");
    SET_SOCKET_OPTION(socket, ZMQ_RECONNECT_IVL,"ZMQ_RECONNECT_IVL");
    SET_SOCKET_OPTION(socket, ZMQ_RECONNECT_IVL_MAX,"ZMQ_RECONNECT_IVL_MAX");
    return 0;
}

int ZMQBaseClass::resetOutputQueue(void *socket,
                                   MapZMQConfiguration &default_conf,
                                   const MapZMQConfiguration &startup_conf) {
    int err = 0;
    int prop_value = 0;
    err = zmq_setsockopt(socket, ZMQ_RCVHWM, &prop_value, sizeof(int));
    if(err == 0) {
        err = setSocketOption(socket,
                              default_conf,
                              startup_conf,
                              ZMQ_RCVHWM,
                              "ZMQ_RCVHWM",
                              "resetOutputQueue");
    }
    return err;
}

int ZMQBaseClass::connectSocket(void *socket,
                                const std::string& connect_url,
                                const std::string& domain) {
    int err = 0;
    if((err = zmq_connect(socket,
                          connect_url.c_str()))) {
        err = zmq_errno();
        ZMQDIO_BASE_LERR_ << CHAOS_FORMAT("Error connecting socket for %1% with error %2%[%3%]",%domain%zmq_strerror(err)%err);
    }
    return err;
}

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
        message_size_read=0;
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
                              void *message_data,
                              size_t message_size,
                              zmq_free_fn *ffn,
                              void *hint,
                              bool more_to_send) {
    int err = 0;
    zmq_msg_t message;
    if((err = zmq_msg_init_data(&message,
                                message_data,
                                message_size,
                                ffn,
                                hint)) == -1){
        //error creating the message
        err = zmq_errno();
        ZMQDIO_BASE_LERR_ << "Error initializing message with error:" << PRINT_ZMQ_ERR(err);
    } else {
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

int ZMQBaseClass::setAndReturnID(void *socket,
                                 std::string& new_id) {
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
                                  DirectIODataPackSPtr& data_pack_handle) {
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
                                  DirectIODataPackSPtr& data_pack_handle) {
    int                     err = 0;
    std::string             empty_delimiter;
    bool                    have_more_message = false;
    size_t                  readed_byte;
    char					header_buffer[DIRECT_IO_HEADER_SIZE];
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
    data_pack_handle.reset(new DirectIODataPack());
    
    //manage little endina conversion for header
    memcpy(&data_pack_handle->header, header_buffer, sizeof(DirectIODataPackDispatchHeader_t));
    DIRECT_IO_DATAPACK_DISPATCH_HEADER_FROM_ENDIAN(data_pack_handle);
    data_pack_handle->header.channel_data_size = DIRECT_IO_GET_CHANNEL_DATA_SIZE(header_buffer);
    data_pack_handle->header.channel_header_size = DIRECT_IO_GET_CHANNEL_HEADER_SIZE(header_buffer);
    
    //check what i need to reice
    switch(data_pack_handle->header.dispatcher_header.fields.channel_part) {
        case DIRECT_IO_CHANNEL_PART_EMPTY:
            break;
        case DIRECT_IO_CHANNEL_PART_HEADER_ONLY:
            
            //check if we have header message
            EXIT_IF_NO_MORE_MESSAGE(-13002, "No other message after header for DIRECT_IO_CHANNEL_PART_HEADER_ONLY");
            
            //init header data buffer
            data_pack_handle->channel_header_data = ChaosMakeSharedPtr<Buffer>(data_pack_handle->header.channel_header_size);
            
            //read the channel header
            if((err = readMessage(socket,
                                  data_pack_handle->channel_header_data->data(),
                                  data_pack_handle->header.channel_header_size,
                                  readed_byte))){
                ZMQDIO_BASE_LERR_<< "Error reading the channel header with code:"<<err;
            }
            break;
        case DIRECT_IO_CHANNEL_PART_DATA_ONLY:
            
            //check if we have data message
            EXIT_IF_NO_MORE_MESSAGE(-13003, "No other message after header for DIRECT_IO_CHANNEL_PART_DATA_ONLY");
            
            //init data buffer
            data_pack_handle->channel_data = ChaosMakeSharedPtr<Buffer>(data_pack_handle->header.channel_data_size);
            
            //read data part
            if((err = readMessage(socket,
                                  data_pack_handle->channel_data->data(),
                                  data_pack_handle->header.channel_data_size,
                                  readed_byte))){
                ZMQDIO_BASE_LERR_<< "Error reading the channel header with code:"<<err;
            }
            break;
        case DIRECT_IO_CHANNEL_PART_HEADER_DATA:
            //check if we have header message
            EXIT_IF_NO_MORE_MESSAGE(-13004, "No other message after header for DIRECT_IO_CHANNEL_PART_HEADER_DATA");
            
            //allocate header buffer
            data_pack_handle->channel_header_data = ChaosMakeSharedPtr<Buffer>(data_pack_handle->header.channel_header_size);
            
            if((err = readMessage(socket,
                                  data_pack_handle->channel_header_data->data(),
                                  data_pack_handle->header.channel_header_size,
                                  readed_byte))){
                ZMQDIO_BASE_LERR_<< "Error reading the channel header with code:"<<err;
            } else {
                //check if we have data message
                EXIT_IF_NO_MORE_MESSAGE(-13005, "No other message after header for DIRECT_IO_CHANNEL_PART_HEADER_DATA");
                
                //allocate data buffer
                data_pack_handle->channel_data = ChaosMakeSharedPtr<Buffer>(data_pack_handle->header.channel_data_size);
                
                //read data part
                if((err = readMessage(socket,
                                      data_pack_handle->channel_data->data(),
                                      data_pack_handle->header.channel_data_size,
                                      readed_byte))){
                    ZMQDIO_BASE_LERR_<< "Error reading the channel header with code:"<<err;
                }
            }
            break;
    }
    return err;
}

int ZMQBaseClass::sendDatapack(void *socket,
                               std::string identity,
                               DirectIODataPackSPtr data_pack) {
    //send identity
    int err = 0;
    if((err = stringSendMore(socket, identity.c_str()))) {
        return err;
    }
    //read the direct io datapack with zmq messages
    return sendDatapack(socket,
                        data_pack);
}

int ZMQBaseClass::sendDatapack(void *socket,
                               DirectIODataPackSPtr data_pack) {
    int err = 0;
    uint16_t sending_opcode = data_pack->header.dispatcher_header.fields.channel_opcode;
    
    //manage little endian conversion
    DIRECT_IO_DATAPACK_DISPATCH_HEADER_TO_ENDIAN(data_pack);
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
                                             (void*)data_pack->channel_header_data->data(),
                                             (size_t)data_pack->header.channel_header_size,
                                             zqmFreeSentData,
                                             new DisposeSentMemoryInfo(data_pack->channel_header_data,
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
                                             data_pack->channel_data->data(),
                                             (size_t)data_pack->header.channel_data_size,
                                             zqmFreeSentData,
                                             new DisposeSentMemoryInfo(data_pack->channel_data,
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
                                             data_pack->channel_header_data->data(),
                                             (size_t)data_pack->header.channel_header_size,
                                             zqmFreeSentData,
                                             new DisposeSentMemoryInfo(data_pack->channel_header_data,
                                                                       DisposeSentMemoryInfo::SentPartHeader,
                                                                       sending_opcode),
                                             true))){
                    ZMQDIO_BASE_LERR_ << "Error sending channel header part:"<< PRINT_ZMQ_ERR(err);
                    //error sending header data
                } else if((err = sendMessage(socket,
                                             data_pack->channel_data->data(),
                                             (size_t)data_pack->header.channel_data_size,
                                             zqmFreeSentData,
                                             new DisposeSentMemoryInfo(data_pack->channel_data,
                                                                       DisposeSentMemoryInfo::SentPartData,
                                                                       sending_opcode),
                                             false))){
                    ZMQDIO_BASE_LERR_ << "Error sending channel data part:"<< PRINT_ZMQ_ERR(err);
                    if(data_pack->channel_data != NULL) {
                        ZMQDIO_BASE_LERR_ << "Free the channel data memory";
                    }
                }
                break;
        }
    }
    return err;
}
