/*
 *	UpdateProperty.h
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

#include <ChaosMetadataServiceClient/api_proxy/node/UpdateProperty.h>

using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

API_PROXY_CD_DEFINITION(UpdateProperty,
                        "system",
                        "updateProperty")

/*!
 
 */
ApiProxyResult UpdateProperty::execute(const std::string& node_unique_id,
                                       const NodePropertyGroupList& node_property_groups_list) {
    std::unique_ptr<CDataWrapper> message(new CDataWrapper());
    //add node uid
    message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_unique_id);
    
    std::unique_ptr<CDataWrapper> update_property_pack(new CDataWrapper());
    //scan all property group
    for(NodePropertyGroupListConstIterator it = node_property_groups_list.begin();
        it != node_property_groups_list.end();
        it++) {
        std::unique_ptr<CDataWrapper> property_group(new CDataWrapper());
        
        //scan all property
        for(ParameterSetterListIterator it_p = (*it)->group_property_list.begin();
            it_p != (*it)->group_property_list.end();
            it_p++) {
            //
            (*it_p)->setTo(*property_group);
        }
        update_property_pack->addCSDataValue((*it)->group_name, *property_group);
    }
    message->addCSDataValue("update_property", *update_property_pack);
    
    //call api
    return callApi(message.release());
}
