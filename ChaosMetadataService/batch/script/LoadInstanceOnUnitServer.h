/*
 *	LoadInstanceOnUnitServer.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 27/06/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_AE318A9D_3558_4D57_95E0_C12899F43BD9_LoadInstanceOnUnitServer_h
#define __CHAOSFramework_AE318A9D_3558_4D57_95E0_C12899F43BD9_LoadInstanceOnUnitServer_h

#include "../mds_service_batch.h"

#include <memory>
namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            namespace script {
                
                class LoadInstanceOnUnitServer:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    
                    std::string unit_server;
                    ChaosStringVector epool_list;
                    std::auto_ptr<RequestInfo> request;
                    std::auto_ptr<CDataWrapper> load_unload_pack;
                public:
                    LoadInstanceOnUnitServer();
                    ~LoadInstanceOnUnitServer();
                protected:
                    // inherited method
                    void setHandler(chaos_data::CDataWrapper *data);
                    
                    // inherited method
                    void acquireHandler();
                    
                    // inherited method
                    void ccHandler();
                    
                    // inherited method
                    bool timeoutHandler();
                };
            }
        }
    }
}
#endif /* __CHAOSFramework_AE318A9D_3558_4D57_95E0_C12899F43BD9_LoadInstanceOnUnitServer_h */
