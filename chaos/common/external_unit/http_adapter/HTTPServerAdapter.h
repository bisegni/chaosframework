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

#ifndef __CHAOSFramework__A9B46FE_F810_4509_990F_9F19087E9AE8_HTTPServerAdapter_h
#define __CHAOSFramework__A9B46FE_F810_4509_990F_9F19087E9AE8_HTTPServerAdapter_h

#include <chaos/common/external_unit/http_adapter/HTTPBaseAdapter.h>
#include <chaos/common/external_unit/AbstractServerAdapter.h>
#include <chaos/common/external_unit/ExternalUnitConnection.h>
namespace chaos{
    namespace common {
        namespace external_unit {
            namespace http_adapter {
                
                struct ServerWorkRequest {
                    const WorkRequestType r_type;
                    const std::string connection_uuid;
                    ChaosUniquePtr<chaos::common::data::CDataBuffer> buffer;
                    
                    ServerWorkRequest():
                    r_type(WorkRequestTypeUnspecified),
                    buffer(){}
                    
                    ServerWorkRequest(const std::string& _connection_uuid,
                                      const char *ptr,
                                      uint32_t size):
                    r_type(WorkRequestTypeWSFrame),
                    connection_uuid(_connection_uuid),
                    buffer(new chaos::common::data::CDataBuffer(ptr, size)){}
                };
                
                
                typedef ChaosSharedPtr<ServerWorkRequest> ServerWorkRequestShrdPtr;
                //!External gateway root class
                class HTTPServerAdapter:
                public AbstractServerAdapter,
                protected CObjectProcessingQueue< ServerWorkRequest >,
                public HTTPBaseAdapter {
                    friend class ExternalUnitConnection;
                    CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ChaosSharedPtr<ExternalUnitConnection>, MapConnection);
                    CHAOS_DEFINE_LOCKABLE_OBJECT(MapConnection, LMapConnection);

                    //!contains all connection
                    LMapConnection  map_connection;

                    bool run;
                    HTTPServerAdapterSetting setting;

                    struct mg_connection *root_connection;
                    struct mg_serve_http_opts s_http_server_opts;

                    void poller();
                    static const std::string getSerializationType(http_message *http_message);
                    static void eventHandler(mg_connection *nc,
                                             int ev,
                                             void *ev_data);
                    void manageWSHandshake(mg_connection *nc,
                                           http_message *message);
                    OpcodeShrdPtr composeAcceptOpcode(const std::string& connection_uuid,
                                                      bool accepted);
                    OpcodeShrdPtr composeCloseOpcode(const std::string& connection_uuid);
                    
                    void consumeConenctionMessageQueue(mg_connection *nc);
                    int _sendDataToConnectionQueue(const std::string& conn_uuid,
                                                   chaos::common::data::CDBufferUniquePtr data,
                                                   const EUCMessageOpcode opcode);
                    void executeOpcodeOnConnection(OpcodeShrdPtr op, mg_connection *nc);
                protected:
                    void processBufferElement(ServerWorkRequest *request, ElementManagingPolicy& policy) throw(CException);
                    int sendDataToConnection(const std::string& connection_identifier,
                                             chaos::common::data::CDBufferUniquePtr data,
                                             const EUCMessageOpcode opcode);
                    int closeConnection(const std::string& connection_identifier);
                public:
                    HTTPServerAdapter();
                    ~HTTPServerAdapter();
                    void init(void *init_data) throw (chaos::CException);
                    void deinit() throw (chaos::CException);
                    int registerEndpoint(ExternalUnitServerEndpoint& endpoint);
                    int deregisterEndpoint(ExternalUnitServerEndpoint& endpoint);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__A9B46FE_F810_4509_990F_9F19087E9AE8_HTTPServerAdapter_h */
