/*
 *	GetDescription.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/unit_server/NewUS.h>

using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::unit_server;

API_PROXY_CD_DEFINITION(NewUS,
                        "unit_server",
                        "newUS")

/*!

 */
ApiProxyResult NewUS::execute(const std::string& new_unit_server_uid,const std::string desc,const chaos::common::data::CDataWrapper*custom) {
    chaos::common::data::CDataWrapper *message = new chaos::common::data::CDataWrapper();
    message->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, new_unit_server_uid);
    message->addStringValue(chaos::NodeDefinitionKey::NODE_DESC, desc);

    return callApi(message);
}
