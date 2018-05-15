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

#ifndef CHAOSFramework_ZMQGlobalUtils_h
#define CHAOSFramework_ZMQGlobalUtils_h

#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/DirectIOForwarder.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <string>
#include <zmq.h>

namespace chaos {
    namespace common {
        namespace direct_io {
            namespace impl {
                
#define ZMQ_DO_AGAIN(x) do{x}while(err == EAGAIN);
                
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, std::string, MapZMQConfiguration)
                
                class ZMQBaseClass {
                protected:
                    
                    int configureContextWithStartupParameter(void *context,
                                                             MapZMQConfiguration &default_conf,
                                                             const MapZMQConfiguration &startup_conf,
                                                             const std::string& domain);
                    int configureSocketWithStartupParameter(void *socket,
                                                            MapZMQConfiguration &default_conf,
                                                            const MapZMQConfiguration &startup_conf,
                                                            const std::string& domain);
                    
                    int connectSocket(void *socket,
                                      const std::string& connect_url,
                                      const std::string& domain);
                    
                    //!
                    int closeSocketNoWhait (void *socket);
                    
                    //! read a new message from zmq socket
                    /*!
                     
                     */
                    inline int readMessage(void *socket,
                                           void *message_data,
                                           size_t message_max_size,
                                           size_t& message_size_read);
                    
                    //! send a new message from zmq socket
                    /*!
                     
                     */
                    inline int sendMessage(void *socket,
                                           void *message_data,
                                           size_t message_size,
                                           zmq_free_fn *ffn,
                                           void *hint,
                                           bool more_to_send);
                    
                    inline int sendMessage(void *socket,
                                           void *message_data,
                                           size_t message_size,
                                           bool more_to_send);
                    
                    //! read a new message from zmq socket
                    /*!
                     
                     */
                    inline int moreMessageToRead(void * socket,
                                                 bool& more_to_read);
                    
                    //! receive a string
                    inline int stringReceive(void *socket,
                                             std::string& received_string);
                    
                    //! send string closing message
                    int stringSend(void *socket,
                                   const char *string);
                    
                    //! send a string with more message option
                    int stringSendMore(void *socket,
                                       const char *string);
                    
                    //! Set a random id on socket
                    int setID(void *socket);
                    
                    //! Set a socket id and return it
                    /*!
                     \param socket, the socket where need to apply the new generate identity
                     \param new_id, return the new generated id
                     */
                    int setAndReturnID(void *socket,
                                       std::string& new_id);
                    
                    //! send the start of the zmq envelop start
                    /*!
                     This method need to be called befor message forwarding,
                     it start the zmq forwarding envelop
                     */
                    inline int sendStartEnvelop(void *socket);
                    
                    //! receive the start of a message envelop
                    /*!
                     Before message can be read, the start of the envelop need to be received
                     */
                    inline int receiveStartEnvelop(void *socket);
                    
                    //! receive Direct io datapack by socket
                    int reveiceDatapack(void *socket,
                                        std::string& identity,
                                        chaos::common::direct_io::DirectIODataPackSPtr& data_pack_handle);
                    //! receive a datapack
                    int reveiceDatapack(void *socket,
                                        chaos::common::direct_io::DirectIODataPackSPtr& data_pack_handle);
                    
                    //! send direct io datapack
                    int sendDatapack(void *socket,
                                     std::string identity,
                                     chaos::common::direct_io::DirectIODataPackSPtr data_pack);
                    
                    //! send dirrect io datapack
                    int sendDatapack(void *socket,
                                     chaos::common::direct_io::DirectIODataPackSPtr data_pack);
                };
                
            }
        }
    }
}



#endif
