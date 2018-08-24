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

#include <chaos_metadata_service_client/api_proxy/script/UpdateScriptOnNode.h>

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
    return callApi(api_data);
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
    return callApi(api_data);
}
