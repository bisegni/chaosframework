/*
 *	HTTPProtocolAdapter.h
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

#ifndef __CHAOSFramework__BDC4C59_4AC1_4BE3_AE95_F497A67E089E_HTTPProtocolAdapter_h
#define __CHAOSFramework__BDC4C59_4AC1_4BE3_AE95_F497A67E089E_HTTPProtocolAdapter_h

#include <chaos_micro_unit_toolkit/connection/protocol_adapter/AbstractProtocolAdapter.h>

#include <chaos_micro_unit_toolkit/external_lib/mongoose.h>

namespace chaos {
    namespace micro_unit_toolkit {
        namespace connection {
            namespace protocol_adapter {
                namespace http {
                    //! Abstract base class for all protocols adapter
                    class HTTPProtocolAdapter:
                    public AbstractProtocolAdapter {
                        struct mg_mgr mgr;
                        struct mg_connection *root_conn;
                        static void ev_handler(struct mg_connection *conn,
                                               int event,
                                               void *event_data);
                        
                        int sendRawMessage(data::DataPackUniquePtr& message);
                    public:
                        HTTPProtocolAdapter(const std::string& endpoint);
                        
                        ~HTTPProtocolAdapter();
                        
                        int connect();
                        
                        int close();
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__BDC4C59_4AC1_4BE3_AE95_F497A67E089E_HTTPProtocolAdapter_h */
