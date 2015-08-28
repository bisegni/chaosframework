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

#include <string>

#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/DirectIOForwarder.h>

namespace chaos {
    namespace common {
        namespace direct_io {
            namespace impl {
                
#define ZMQ_DO_AGAIN(x) do{x}while(err == EAGAIN);
                
                class ZMQBaseClass {
                protected:
                    //!
                    int closeSocketNoWhait (void *socket);
                    
                    //!
                    int stringReceive(void *socket,
                                      std::string& received_string);
                    
                    //!
                    int stringSend(void *socket,
                                   const char *string);
                    
                    //!
                    int stringSendMore(void *socket,
                                       const char *string);
                    
                    //!
                    int setID(void *socket);
                    
                    //!
                    int setAndReturnID(void *socket,
                                       std::string& new_id);
                    
                    int sendStartEnvelop(void *socket);
                    
                    int receiveStartEnvelop(void *socket);
                    
                    //! receive Direct io datapack by socket
                    int reveiceDatapack(void *socket,
                                        std::string& identity,
                                        DirectIODataPack **data_pack_handle);
                    
                    int sendDatapack(void *socket,
                                     std::string identity,
                                     DirectIODataPack *data_pack,
                                     DirectIOClientDeallocationHandler *header_deallocation_handler,
                                     DirectIOClientDeallocationHandler *data_deallocation_handler);
                };
                
            }
        }
    }
}



#endif
