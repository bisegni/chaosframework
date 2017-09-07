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

#include "GetInstance.h"

#include <chaos/common/property/property.h>

#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::common::property;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_GI_INFO INFO_LOG(GetInstance)
#define CU_GI_DBG  DBG_LOG(GetInstance)
#define CU_GI_ERR  ERR_LOG(GetInstance)

GetInstance::GetInstance():
AbstractApi("getInstance"){
    
}

GetInstance::~GetInstance() {
    
}

CDataWrapper *GetInstance::execute(CDataWrapper *api_data,
                                   bool& detach_data) throw(chaos::CException) {
    
    if(!api_data) {LOG_AND_TROW(CU_GI_ERR, -1, "Search parameter are needed");}
    if(!api_data->hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) {LOG_AND_TROW(CU_GI_ERR, -2, "The ndk_unique_id key (representing the control unit uid) is mandatory");}
    
    int err = 0;
    CDataWrapper *result = NULL;
    const std::string cu_uid = api_data->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
    
    GET_DATA_ACCESS(NodeDataAccess, n_da, -3)
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -4)
    if((err = cu_da->getInstanceDescription(cu_uid,
                                            &result))){
        LOG_AND_TROW(CU_GI_ERR, err, boost::str(boost::format("Error fetching the control unit instance description for cuid:%1%") % cu_uid));
    }
    
    //for compativbility  update here the default porperty values
    PropertyGroupVector pgv;
    if((err = n_da->getPropertyDefaultValue(cu_uid,
                                            pgv))) {
        LOG_AND_TROW(CU_GI_ERR, err, CHAOS_FORMAT("Error reading property defaults for node:%1%",%cu_uid));
    }
    if(pgv.size()) {
        PropertyGroup& pg = pgv[0];
        if(pg.hasProperty(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY)) {
            result->addVariantValue(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY, pg.getProperty(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY).getPropertyValue());
        }
        if(pg.hasProperty(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE)) {
            result->addVariantValue(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE, pg.getProperty(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE).getPropertyValue());
        }
        if(pg.hasProperty(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_AGEING)) {
            result->addVariantValue(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_AGEING, pg.getProperty(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_AGEING).getPropertyValue());
        }
        if(pg.hasProperty(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME)) {
            result->addVariantValue(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME, pg.getProperty(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME).getPropertyValue());
        }
        if(pg.hasProperty(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME)) {
            result->addVariantValue(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME, pg.getProperty(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME).getPropertyValue());
        }
    }
    return result;
}
