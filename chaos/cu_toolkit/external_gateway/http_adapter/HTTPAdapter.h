/*
 *	HTTPAdapter.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 22/06/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__A9B46FE_F810_4509_990F_9F19087E9AE8_HTTPAdapter_h
#define __CHAOSFramework__A9B46FE_F810_4509_990F_9F19087E9AE8_HTTPAdapter_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/pqueue/CObjectProcessingQueue.h>

#include <chaos/cu_toolkit/additional_lib/mongoose.h>
#include <chaos/cu_toolkit/external_gateway/AbstractAdapter.h>
#include <chaos/cu_toolkit/external_gateway/http_adapter/http_adapter_types.h>
#include <chaos/cu_toolkit/external_gateway/http_adapter/HTTPExternalUnitConnection.h>

#include <boost/thread.hpp>

namespace chaos{
    namespace cu {
        namespace external_gateway {
            namespace http_adapter {
                
                CHAOS_DEFINE_MAP_FOR_TYPE(uintptr_t, ChaosSharedPtr<HTTPExternalUnitConnection>, MapConnection);
                CHAOS_DEFINE_LOCKABLE_OBJECT(MapConnection, LMapConnection);
                
                //!External gateway root class
                class HTTPAdapter:
                protected CObjectProcessingQueue< WorkRequest >,
                public AbstractAdapter {
                    bool run;
                    HttpAdapterSetting setting;
                    
                    struct mg_serve_http_opts s_http_server_opts;
                    
                    ChaosUniquePtr<boost::thread> thread_poller;
                    
                    struct mg_mgr mgr;
                    struct mg_connection *root_connection;
                    
                    //!contains all connection
                    LMapConnection  map_connection;
                    
                    void poller();
                    static const std::string getSerializationType(http_message *http_message);
                    static void eventHandler(mg_connection *nc, int ev, void *ev_data);
                protected:
                    void processBufferElement(WorkRequest *request, ElementManagingPolicy& policy) throw(CException);
                public:
                    HTTPAdapter();
                    ~HTTPAdapter();
                    void init(void *init_data) throw (chaos::CException);
                    void deinit() throw (chaos::CException);
                    int registerEndpoint(ExternalUnitEndpoint& endpoint);
                    int deregisterEndpoint(ExternalUnitEndpoint& endpoint);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__A9B46FE_F810_4509_990F_9F19087E9AE8_HTTPAdapter_h */
