/*
 *	ManageScriptInstance.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 15/06/16 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/script/ManageScriptInstance.h>

using namespace chaos::common::data;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::script;

API_PROXY_CD_DEFINITION(ManageScriptInstance, "script", "manageScriptInstance")

ApiProxyResult ManageScriptInstance::execute(const std::string& script_name,
                                             const std::string& instance_name,
                                             const bool create) {
    std::auto_ptr<CDataWrapper> api_data(new CDataWrapper());
    api_data->addStringValue("script_name", script_name);
    api_data->addStringValue("instance_name", instance_name);
    api_data->addBoolValue("create", create);
    return callApi(api_data.release());
}

ApiProxyResult ManageScriptInstance::execute(const std::string& script_name,
                                             const ChaosStringList& instance_names,
                                             const bool create) {
    std::auto_ptr<CDataWrapper> api_data(new CDataWrapper());
    api_data->addStringValue("script_name", script_name);
    for(ChaosStringListConstIterator it = instance_names.begin(),
        end = instance_names.end();
        it != end;
        it++) {
        api_data->appendStringToArray(*it);
    }
    api_data->finalizeArrayForKey("instance_name");
    api_data->addBoolValue("create", create);
    return callApi(api_data.release());
}
