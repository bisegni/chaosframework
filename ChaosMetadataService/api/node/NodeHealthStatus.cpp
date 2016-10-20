/*
 *	NodeHealthStatus.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/10/2016 INFN, National Institute of Nuclear Physics
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

#include "NodeHealthStatus.h"
#include <chaos/common/global.h>

#define NHS_INFO INFO_LOG(NodeRegister)
#define NHS_DBG  DBG_LOG(NodeRegister)
#define NHS_ERR  ERR_LOG(NodeRegister)

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

NodeHealthStatus::NodeHealthStatus():
AbstractApi(chaos::MetadataServerNodeDefinitionKeyRPC::ACTION_NODE_HEALTH_STATUS){}

NodeHealthStatus::~NodeHealthStatus() {}

chaos::common::data::CDataWrapper *NodeHealthStatus::execute(chaos::common::data::CDataWrapper *api_data,
                                                             bool& detach_data) throw (chaos::CException){
    CHECK_CDW_THROW_AND_LOG(api_data, NHS_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, DataPackCommonKey::DPCK_DEVICE_ID, NHS_ERR, -2, CHAOS_FORMAT("The %1% key is mandatory",%DataPackCommonKey::DPCK_DEVICE_ID));
    CHECK_KEY_THROW_AND_LOG(api_data, NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP, NHS_ERR, -3, CHAOS_FORMAT("The %1% key is mandatory",%NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP));
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    
    int err = 0;
    HealthStatSDWrapper attribute_reference_wrapper;
    
    attribute_reference_wrapper.deserialize(api_data);
    const std::string node_uid = api_data->getStringValue(DataPackCommonKey::DPCK_DEVICE_ID);
    if((err = n_da->setNodeHealthStatus(node_uid,
                                        attribute_reference_wrapper()))){
        LOG_AND_TROW(NHS_ERR, -4, CHAOS_FORMAT("Error setting healt stat for node %1%", %node_uid));
        
    }
    return NULL;
}
