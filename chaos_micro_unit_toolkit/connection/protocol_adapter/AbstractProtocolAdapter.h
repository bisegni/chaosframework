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
#include <map>

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
                
                
                //! Abstract base class for all protocols adapter
                class AbstractProtocolAdapter {
                protected:
                    virtual int sendRawMessage(chaos::micro_unit_toolkit::data::DataPackUniquePtr& message) = 0;
                    
                    void handleReceivedMessage(chaos::micro_unit_toolkit::data::DataPackSharedPtr& received_message);
                public:
                    const ProtocolType  protocol_type;
                    const std::string   protocol_endpoint;
                    
                    AbstractProtocolAdapter(ProtocolType _impl_type,
                                            const std::string& connection_endpoint);
                    virtual ~AbstractProtocolAdapter();
                    
                    virtual int connect() = 0;
                    
                    int sendMessage(data::DataPackUniquePtr& message);
                    
                    int sendRequest(data::DataPackUniquePtr& message,
                                    uint32_t& request_id);
                    
                    bool hasMoreMessage();
                    
                    data::DataPackSharedPtr getNextMessage();
                    
                    bool hasResponse();
                    
                    bool hasResponseAvailable(uint32_t request_id);
                    
                    data::DataPackSharedPtr retrieveRequestResponse(uint32_t request_id);
                    
                    virtual int close() = 0;
                protected:
                    typedef std::map<uint32_t, data::DataPackSharedPtr> MapRequestIDResponse;
                    typedef std::pair<uint32_t, data::DataPackSharedPtr> MapRequestIDResponsePair;
                    typedef MapRequestIDResponse::iterator MapRequestIDResponseIterator;

                    uint32_t adapter_request_id;
                    ConnectionState connection_status;
                    MapRequestIDResponse                map_req_id_response;
                    std::queue<data::DataPackSharedPtr> queue_received_messages;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_CD2B55D0_1C87_4434_BADC_420C3CB85F42_AbstractProtocolAdapter_h */
