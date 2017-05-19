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

#include <ChaosMetadataServiceClient/api_proxy/script/UpdateScriptOnNode.h>

#include <chaos/common/chaos_constants.h>

using namespace chaos::common::data;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::script;

API_PROXY_CD_DEFINITION(UpdateScriptOnNode, "script", "updateScriptOnNode")

ApiProxyResult UpdateScriptOnNode::execute(const std::string& target_node,
                                           const ScriptBaseDescription& sbd) {
    ScriptBaseDescriptionSDWrapper swd(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(ScriptBaseDescription, const_cast<ScriptBaseDescription&>(sbd)));
    ChaosUniquePtr<chaos::common::data::CDataWrapper> api_data(new CDataWrapper());
    api_data->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, target_node);
    api_data->appendAllElement(*swd.serialize());
    return callApi(api_data.release());
}

ApiProxyResult UpdateScriptOnNode::execute(const ChaosStringVector& instance_names,
                                           const ScriptBaseDescription& sbd) {
    ChaosUniquePtr<chaos::common::data::CDataWrapper> api_data(new CDataWrapper());
    ScriptBaseDescriptionSDWrapper swd(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(ScriptBaseDescription, const_cast<ScriptBaseDescription&>(sbd)));
    for(ChaosStringVectorConstIterator it = instance_names.begin(),
        end = instance_names.end();
        it != end;
        it++) {
        api_data->appendStringToArray(*it);
    }
    api_data->finalizeArrayForKey(NodeDefinitionKey::NODE_UNIQUE_ID);
    api_data->appendAllElement(*swd.serialize());
    return callApi(api_data.release());
}
