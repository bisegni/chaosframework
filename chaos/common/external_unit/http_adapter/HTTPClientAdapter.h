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

#ifndef chaos_common_external_unit_http_adapter_HTTPClientAdapter_h
#define chaos_common_external_unit_http_adapter_HTTPClientAdapter_h

#include <chaos/common/external_unit/http_adapter/HTTPBaseAdapter.h>
#include <chaos/common/external_unit/ExternalUnitClientEndpoint.h>
#include <chaos/common/external_unit/AbstractClientAdapter.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataBuffer.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/LockableObject.h>

#include <queue>

namespace chaos {
    namespace common {
        namespace external_unit {
            namespace http_adapter {
                class HTTPClientAdapter:
                public HTTPBaseAdapter,
                public AbstractClientAdapter {
                    
                    struct ConnectionInfo {
                        bool connected;
                        std::string endpoint_url;
                        uint64_t next_reconnection_retry_ts;
                        const std::string connection_id;
                        ChaosSharedMutex smux;
                        HTTPClientAdapter *class_instance;
                        struct mg_connection *conn;
                        ChaosSharedPtr<ExternalUnitConnection> ext_unit_conn;
                        
                        ConnectionInfo():
                        connected(false),
                        next_reconnection_retry_ts(0),
                        connection_id(chaos::common::utility::UUIDUtil::generateUUIDLite()),
                        class_instance(NULL),
                        conn(NULL),
                        ext_unit_conn(){}
                    };
                    
                    typedef ChaosSharedPtr<ConnectionInfo> ConnectionInfoShrdPtr;
                    
                    CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ConnectionInfoShrdPtr, MapReconnectionInfo);
                    CHAOS_DEFINE_LOCKABLE_OBJECT(MapReconnectionInfo, LMapReconnectionInfo);
                    
                    bool run;
                    struct mg_mgr mgr;
                    
                    //!map that hold the connection to use
                    LMapReconnectionInfo map_connection;
                    
                    void poller();
                    static void ev_handler(struct mg_connection *conn,
                                           int event,
                                           void *event_data);
                    
                protected:
                    int sendDataToConnection(const std::string& connection_identifier,
                                             const chaos::common::data::CDBufferUniquePtr data,
                                             const EUCMessageOpcode opcode);
                    int closeConnection(const std::string& connection_identifier);
                public:
                    HTTPClientAdapter();
                    ~HTTPClientAdapter();
                    void init(void *init_data) throw (chaos::CException);
                    void deinit() throw (chaos::CException);
                    int addNewConnectionForEndpoint(ExternalUnitClientEndpoint *endpoint,
                                                       const std::string& endpoint_url,
                                                       const std::string& serialization);
                    int removeConnectionsFromEndpoint(ExternalUnitClientEndpoint *target_endpoint);
                };
            }
        }
    }
}

#endif /* chaos_common_external_unit_http_adapter_HTTPClientAdapter_h */
