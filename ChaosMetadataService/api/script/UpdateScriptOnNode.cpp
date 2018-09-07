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

CHAOS_MDS_DEFINE_API_CLASS_CD(UpdateScriptOnNode, "updateScriptOnNode")

CDWUniquePtr UpdateScriptOnNode::execute(CDWUniquePtr api_data) {
    CHECK_CDW_THROW_AND_LOG(api_data, ERR, -1, "No parameter found");
    CHECK_KEY_THROW_AND_LOG(api_data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -2, CHAOS_FORMAT("The %1% key is mandatory",%chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
    
    CreateNewDataWrapper(batch_data,);
    
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
    return CDWUniquePtr();
}
