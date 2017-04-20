/*
 *	UpdateScriptOnNode.cpp
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

#include "UpdateScriptOnNode.h"
#include "../../batch/script/UpdateScriptOnNode.h"

#include <chaos/common/utility/TimingUtil.h>

#include <chaos_service_common/data/data.h>

#define INFO INFO_LOG(UpdateScriptOnNode)
#define DBG  DBG_LOG(UpdateScriptOnNode)
#define ERR  ERR_LOG(UpdateScriptOnNode)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::utility;
using namespace chaos::common::event::channel;
using namespace chaos::common::data::structured;

using namespace chaos::service_common::data::script;

using namespace chaos::metadata_service::api::script;
using namespace chaos::metadata_service::persistence::data_access;

UpdateScriptOnNode::UpdateScriptOnNode():
AbstractApi("updateScriptOnNode"){}

UpdateScriptOnNode::~UpdateScriptOnNode() {}

chaos::common::data::CDataWrapper *UpdateScriptOnNode::execute(CDataWrapper *api_data, bool& detach_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The %1% key is mandatory",%chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
    
    std::auto_ptr<CDataWrapper> batch_data(api_data);
    detach_data = true;
    
    //deserializ base description of the script
    ScriptBaseDescriptionSDWrapper script_description_sdw(batch_data.get());
    if(script_description_sdw().unique_id == 0 ||
       script_description_sdw().name.size() == 0) {
        LOG_AND_TROW(ERR, -3, "Invalid script description");
    }
    //load operation is done in the batch system
    getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::script::UpdateScriptOnNode),
                                      batch_data.release(),
                                      0,
                                      1000);
    return NULL;
}
