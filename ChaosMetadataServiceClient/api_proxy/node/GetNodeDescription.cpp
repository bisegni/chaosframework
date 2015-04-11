/*
 *	GetNodeDescription.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/api_proxy/node/GetNodeDescription.h>
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

API_PROXY_CD_DEFINITION(GetNodeDescription,
                        "system",
                        "getNodeDescription")

/*!

 */
ApiProxyResult GetNodeDescription::execute(const std::string& unique_node_id) {
    chaos::common::data::CDataWrapper *message = new chaos::common::data::CDataWrapper();
    message->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, unique_node_id);
    return callApi(message);
}