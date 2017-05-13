/*
 *	UpdateScriptOnNode.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 20/04/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__A864E9F_AEF8_414D_A76A_0270229F303F_UpdateScriptOnNode_h
#define __CHAOSFramework__A864E9F_AEF8_414D_A76A_0270229F303F_UpdateScriptOnNode_h
#include "../mds_service_batch.h"
#include "../../../chaos_service_common/data/data.h"
#include <memory>
namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            namespace script {
                
                class UpdateScriptOnNode:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    
                    typedef enum UploadPhase {
                        UploadPhaseFetchNodeInfo,
                        UploadPhaseSendToNode
                    } UploadPhase;
                    
                    UploadPhase phase;
                    chaos::service_common::data::script::ScriptSDWrapper sdw;
                    ChaosStringVector node_to_update_vec;
                    ChaosStringVectorIterator current_node_it;
                    
                    std::unique_ptr<RequestInfo> request;
                    std::unique_ptr<CDataWrapper> script_update_data_pack;
                public:
                    UpdateScriptOnNode();
                    ~UpdateScriptOnNode();
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
                     \param node_uid is the name of the scrip instance that is a chaos node
                     \ return true if operation is gone well, false if load of instance need to abort
                     */
                    bool prepareScriptInstance(const chaos::service_common::data::script::ScriptBaseDescription& current_script_description,
                                               const std::string& node_uid);
                    
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
#endif /* __CHAOSFramework__A864E9F_AEF8_414D_A76A_0270229F303F_UpdateScriptOnNode_h */
