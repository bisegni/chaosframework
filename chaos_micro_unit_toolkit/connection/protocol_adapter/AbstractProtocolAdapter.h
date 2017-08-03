/*
 *	AbstractProtocolAdapter.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/08/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_CD2B55D0_1C87_4434_BADC_420C3CB85F42_AbstractProtocolAdapter_h
#define __CHAOSFramework_CD2B55D0_1C87_4434_BADC_420C3CB85F42_AbstractProtocolAdapter_h

#include <chaos_micro_unit_toolkit/data/DataPack.h>
#include <chaos_micro_unit_toolkit/connection/connection_type.h>

#include <string>
#include <queue>

namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace protocol_adapter {
                
                typedef enum {
                    ConnectionStateDisconnected,
                    ConnectionStateConnecting,
                    ConnectionStateConnected,
                    ConnectionStateConnectionError
                } ConnectionState;
                
                class ProtocolAdapterHandler {
                public:
                    virtual int messageReceived(data::DataPackUniquePtr message);
                };
                
                //! Abstract base class for all protocols adapter
                class AbstractProtocolAdapter {
                    ProtocolAdapterHandler *handler;
                public:
                    const ProtocolType  protocol_type;
                    const std::string   protocol_endpoint;
                    
                    AbstractProtocolAdapter(ProtocolType _impl_type,
                                            const std::string& connection_endpoint);
                    virtual ~AbstractProtocolAdapter();
                    
                    virtual int connect() = 0;
                    
                    virtual int sendMessage(data::DataPackUniquePtr message) = 0;
                    
                    virtual data::DataPackSharedPtr readMessage() = 0;
                    
                    virtual bool hasMoreMessage() = 0;
                    
                    virtual int close() = 0;
                    
                    void setHandler(ProtocolAdapterHandler *handler);
                protected:
                    ConnectionState connection_status;
                    std::queue<data::DataPackSharedPtr> queue_received_messages;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_CD2B55D0_1C87_4434_BADC_420C3CB85F42_AbstractProtocolAdapter_h */
