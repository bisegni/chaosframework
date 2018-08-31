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

CHAOS_MDS_DECLARE_API_CD_STR(NodeHealthStatus, chaos::MetadataServerNodeDefinitionKeyRPC::ACTION_NODE_HEALTH_STATUS)

CDWUniquePtr NodeHealthStatus::execute(CDWUniquePtr api_data) throw (chaos::CException){
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
    return CDWUniquePtr();
}
