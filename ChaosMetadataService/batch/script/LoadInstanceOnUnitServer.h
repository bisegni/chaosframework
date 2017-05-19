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
#include "../../../chaos_service_common/data/data.h"
#include <memory>
namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            namespace script {
                
                class LoadInstanceOnUnitServer:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    
                    typedef enum SearchScriptPhase {
                        SearchScriptPhaseLoadScriptPage,
                        SearchScriptPhaseLoadInstancePage,
                        SearchScriptPhaseConsumeInstance
                    } SearchScriptPhase;
                    
                    typedef enum InstanceWorkPhase {
                        InstanceWorkPhasePrepare,
                        InstanceWorkPhaseLoadOnServer
                    } InstanceWorkPhase;
                    
                    std::string unit_server;
                    std::string unit_server_rpc_addr;
                    
                    ChaosStringVector epool_list;
                    ChaosUniquePtr<RequestInfo> request;
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> load_datapack;
                    
                    uint64_t                last_sequence_id;
                    SearchScriptPhase       search_script_phase;
                    InstanceWorkPhase       instance_work_phase;
                    
                    uint32_t current_script_idx;
                    uint32_t current_instance_idx;
                    
                    ChaosStringVector current_instance_page;
                    std::vector<chaos::service_common::data::script::ScriptBaseDescription> current_script_page;
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
                    
                    //! try to prepare the messag efor load instance on server
                    /*!
                     \param current_instance is the name of the scrip instance that is a chaos node
                     \ return true if operation is gone well, false if load of instance need to abort
                     */
                    bool prepareScriptInstance(const chaos::service_common::data::script::ScriptBaseDescription& current_script_description,
                                               const std::string& current_instance_name);
                    
                    //! try to load curren tinstance on the unit server
                    /*!
                     \param current_instance is the name of the scrip instance that is a chaos node
                     \ return true if operation is terminated, false if the opreation need more time
                     */
                    bool loadScriptInstance();
                };
            }
        }
    }
}
#endif /* __CHAOSFramework_AE318A9D_3558_4D57_95E0_C12899F43BD9_LoadInstanceOnUnitServer_h */
