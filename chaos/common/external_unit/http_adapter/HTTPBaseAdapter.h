/*
 * Copyright 2012, 10/10/2017 INFN
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

#ifndef chaos_common_external_unit_http_adapter_HTTPBaseAdapter_h
#define chaos_common_external_unit_http_adapter_HTTPBaseAdapter_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Bimap.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/pqueue/CObjectProcessingQueue.h>

#include <chaos/common/additional_lib/mongoose.h>
#include <chaos/common/external_unit/http_adapter/http_adapter_types.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace external_unit {
            namespace http_adapter {
                
                class HTTPBaseAdapter {
                    friend class ExternalUnitConnection;
                protected:
                    ChaosUniquePtr<boost::thread> thread_poller;
                    
                    virtual int sendDataToConnection(const std::string& connection_identifier,
                                                     const chaos::common::data::CDBufferUniquePtr data,
                                                     const EUCMessageOpcode opcode) = 0;
                    
                    virtual int closeConnection(const std::string& connection_identifier) = 0;
                    
                    void sendHTTPJSONError(mg_connection *nc,
                                           int status_code,
                                           const int error_code,
                                           const std::string& error_message){
                        chaos::common::data::CDataWrapper err_data_pack;
                        err_data_pack.addInt32Value("error_code", error_code);
                        err_data_pack.addStringValue("error_message", error_message);
                        const std::string json_error = err_data_pack.getJSONString();
                        mg_send_head(nc, 400, 0, "Content-Type: application/json");
                        mg_printf(nc, "%s", json_error.c_str());
                    }
                    
                    void sendWSJSONError(mg_connection *nc,
                                         const int error_code,
                                         const std::string& error_message,
                                         bool close_connection = false){
                        chaos::common::data::CDataWrapper err_data_pack;
                        err_data_pack.addInt32Value("error_code", error_code);
                        err_data_pack.addStringValue("error_message", error_message);
                        const std::string json_error = err_data_pack.getJSONString();
                        mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, json_error.c_str(), json_error.size());
                        if(close_connection){mg_send_websocket_frame(nc, WEBSOCKET_OP_CLOSE, NULL, 0);}
                    }
                    
                public:
                    HTTPBaseAdapter(){}
                    virtual ~HTTPBaseAdapter(){}
                };
            }
        }
    }
}

#endif /* chaos_common_external_unit_http_adapter_HTTPBaseAdapter_h */

