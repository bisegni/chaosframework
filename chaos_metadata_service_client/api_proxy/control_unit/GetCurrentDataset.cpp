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

#include <chaos_metadata_service_client/api_proxy/control_unit/GetCurrentDataset.h>

using namespace chaos::common::data;
using namespace chaos::common::data::structured;

using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::control_unit;

API_PROXY_CD_DEFINITION(GetCurrentDataset,
                        "control_unit",
                        "getCurrentDataset")

/*!
 
 */
ApiProxyResult GetCurrentDataset::execute(const std::string& cu_unique_id) {
    CDWUniquePtr message(new CDataWrapper());
    message->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, cu_unique_id);
    return callApi(message);
}

void GetCurrentDataset::deserialize(chaos::common::data::CDataWrapper& cdw,
                                    chaos::common::data::structured::DatasetAttributeList& dataset_attribute_list) {
    if(!cdw.hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION) ||
       !cdw.isVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION)) return;
    
    CMultiTypeDataArrayWrapperSPtr d_attr_vec = cdw.getVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
    for(int idx = 0;
        idx < d_attr_vec->size();
        idx++) {
        DatasetAttributeSDWrapper da_sdw;
        
        CDWUniquePtr attr(d_attr_vec->getCDataWrapperElementAtIndex(idx));
        da_sdw.deserialize(attr.get());
        dataset_attribute_list.push_back(da_sdw());
    }
    
}

