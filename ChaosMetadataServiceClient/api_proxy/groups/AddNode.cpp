/*
 *	AddNode.cpp
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

#include <ChaosMetadataServiceClient/api_proxy/groups/AddNode.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::groups;

API_PROXY_CD_DEFINITION(AddNode,
                        "groups",
                        "addNode")
/*!
 
 */
ApiProxyResult AddNode::execute(const std::string& node_domain,
                                const std::string& node_name,
                                const std::string& node_parent_path) {
    
    std::auto_ptr<CDataWrapper> pack(new CDataWrapper());
    
    pack->addStringValue("node_name", node_name);
    pack->addStringValue("node_domain", node_domain);
    if(node_parent_path.size()){pack->addStringValue("node_parent_path", node_parent_path);}
    
    return callApi(pack.release());
}
