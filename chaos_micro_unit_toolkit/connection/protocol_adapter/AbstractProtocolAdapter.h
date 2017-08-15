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
                    ConnectionStateAccepted,
                    ConnectionStateNotAccepted,
                    ConnectionStateConnectionError
                } ConnectionState;
                
                
                //! Abstract base class for all protocols adapter
                class AbstractProtocolAdapter {
                protected:
                    virtual int sendRawMessage(chaos::micro_unit_toolkit::data::DataPackUniquePtr& message) = 0;
                    
                    void handleReceivedMessage(chaos::micro_unit_toolkit::data::DataPackSharedPtr& received_message);
                public:
                    const std::string   protocol_endpoint;
                    const std::string   protocol_option;
                    AbstractProtocolAdapter(const std::string& _connection_endpoint,
                                            const std::string& _protocol_option);
                    virtual ~AbstractProtocolAdapter();
                    
                    virtual int connect() = 0;
                    
                    //!execute al llpending operation and waith a number milliseconds for incoming message
                    
                    virtual void poll(int32_t milliseconds_wait = 100) = 0;
                    
                    virtual int close() = 0;

                    const ConnectionState& getConnectionState() const;
                    
                    
                    int sendMessage(data::DataPackUniquePtr& message);
                    
                    int sendRequest(data::DataPackUniquePtr& message,
                                    uint32_t& request_id);
                    
                    bool hasMoreMessage();
                    
                    data::DataPackSharedPtr getNextMessage();
                    
                    bool hasResponse();
                    
                    bool hasResponseAvailable(uint32_t request_id);
                    
                    data::DataPackSharedPtr retrieveRequestResponse(uint32_t request_id);
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
