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

#ifndef CHAOSFramework_ZMQGlobalUtils_h
#define CHAOSFramework_ZMQGlobalUtils_h

#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/DirectIOForwarder.h>

#include <string>
#include <zmq.h>

namespace chaos {
    namespace common {
        namespace direct_io {
            namespace impl {
                
#define ZMQ_DO_AGAIN(x) do{x}while(err == EAGAIN);
                
                class ZMQBaseClass {
                protected:
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
                                           void **message_data,
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
                                        DirectIODataPack **data_pack_handle);
                    //! receive a datapack
                    int reveiceDatapack(void *socket,
                                        DirectIODataPack **data_pack_handle);
                    
                    //! send direct io datapack
                    int sendDatapack(void *socket,
                                     std::string identity,
                                     DirectIODataPack *data_pack,
                                     DirectIODeallocationHandler *header_deallocation_handler,
                                     DirectIODeallocationHandler *data_deallocation_handler);
                    
                    //! send dirrect io datapack
                    int sendDatapack(void *socket,
                                     DirectIODataPack *data_pack,
                                     DirectIODeallocationHandler *header_deallocation_handler,
                                     DirectIODeallocationHandler *data_deallocation_handler);
                };
                
            }
        }
    }
}



#endif
