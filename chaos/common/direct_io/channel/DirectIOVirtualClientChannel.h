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
#ifndef __CHAOSFramework__DirectIOVirtualClientChannel__
#define __CHAOSFramework__DirectIOVirtualClientChannel__

#include <stdint.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/direct_io/DirectIOForwarder.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualChannel.h>

namespace chaos {
    namespace common {
        namespace direct_io {
            class DirectIOClient;
            class DirectIOForwarder;
            class DirectIOClientConnection;
            
            struct DirectIODataPack;
            
            //! forward declaration
            class DirectIODeallocationHandler;
            
            namespace channel {
                
                CHAOS_DEFINE_LOCKABLE_OBJECT(DirectIOForwarder*, DirectIOForwarderPtrLO);
                
                class DirectIOVirtualClientChannel :
                protected DirectIOVirtualChannel,
                protected DirectIODeallocationHandler {
                    friend class chaos::common::direct_io::DirectIOClientConnection;
                    DirectIOForwarderHandler  forward_handler;
                protected:
                    
                    //! subclass can override this with custom persisint() after channel allocation) implementation
                    DirectIODeallocationHandler *header_deallocator;
                    
                    //!only virtual channel class can access this class to permit
                    //! the regulatio of the call
                    DirectIOForwarderPtrLO client_instance;
                    
                    //priority socket
                    int sendPriorityData(chaos::common::direct_io::DirectIODataPackSPtr data_pack);
                    int sendPriorityData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                         chaos::common::direct_io::DirectIODataPackSPtr& asynchronous_answer);
                    //service socket
                    int sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack);
                    int sendServiceData(chaos::common::direct_io::DirectIODataPackSPtr data_pack,
                                        chaos::common::direct_io::DirectIODataPackSPtr& asynchronous_answer);
                    // prepare header for defaut connection data
                    void completeChannnelDataPack(DirectIODataPack *data_pack, bool synchronous_answer = false) {
                        //complete the datapack
                        data_pack->header.dispatcher_header.fields.synchronous_answer = synchronous_answer;
                        data_pack->header.dispatcher_header.fields.channel_idx = channel_route_index;
                    }
                    
                    DirectIOVirtualClientChannel(std::string channel_name, uint8_t channel_route_index);
                    ~DirectIOVirtualClientChannel();
                    
                    //! default header deallocator implementation
                    void freeSentData(void* sent_data_ptr, DisposeSentMemoryInfo *free_info_ptr);
                public:
                    
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DirectIOVirtualChannel__) */
