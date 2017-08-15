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
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message(new CDataWrapper());
    //add node uid
    message->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, node_unique_id);
    
    ChaosUniquePtr<chaos::common::data::CDataWrapper> update_property_pack(new CDataWrapper());
    //scan all property group
    for(NodePropertyGroupListConstIterator it = node_property_groups_list.begin();
        it != node_property_groups_list.end();
        it++) {
        ChaosUniquePtr<chaos::common::data::CDataWrapper> property_group(new CDataWrapper());
        
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
