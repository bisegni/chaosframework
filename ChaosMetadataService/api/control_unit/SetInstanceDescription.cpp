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
#include "SetInstanceDescription.h"
#include <boost/format.hpp>

using namespace chaos::common::data;
using namespace chaos::common::property;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CUCUI_INFO INFO_LOG(SetInstanceDescription)
#define CUCUI_DBG  DBG_LOG(SetInstanceDescription)
#define CUCUI_ERR  ERR_LOG(SetInstanceDescription)

CHAOS_MDS_DEFINE_API_CLASS_CD(SetInstanceDescription, "setInstanceDescription")

CDWUniquePtr SetInstanceDescription::execute(CDWUniquePtr api_data) {
    int err = 0;
    bool presence = false;
    if(!api_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        LOG_AND_TROW(CUCUI_ERR, -1, "Node unique id is mandatory")
    }
    if(!api_data->hasKey("instance_description")) {
        LOG_AND_TROW(CUCUI_ERR, -2, "the instance description need to be associated to a the key 'instance_description'");
    }
    const std::string cu_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
        //!get the unit server data access
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -3)
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4)
    if((err = n_da->checkNodePresence(presence,
                                      cu_uid))){
        LOG_AND_TROW(CUCUI_ERR, err, "Error checking node presence")
    }
    if (!presence) {
        ChaosUniquePtr<chaos::common::data::CDataWrapper> node_min_dec(new CDataWrapper());
        node_min_dec->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, cu_uid);
        node_min_dec->addStringValue(NodeDefinitionKey::NODE_TYPE, api_data->getStringValue(NodeDefinitionKey::NODE_TYPE));
            //need to be create a new empty node
        if((err = cu_da->insertNewControlUnit(*node_min_dec.get()))) {
            LOG_AND_TROW(CUCUI_ERR, err, "Error during new node creation")
        }
    }

        //set the instance whiting the control unit node
        //create the subobject for the instance that need to be insert within the global node
    ChaosUniquePtr<chaos::common::data::CDataWrapper> instance_desc(api_data->getCSDataValue("instance_description"));
        //insert the instance
    if(!instance_desc->hasKey(chaos::NodeDefinitionKey::NODE_PARENT)) {
        LOG_AND_TROW(CUCUI_ERR, -5, "The instance description need to have the unique id of the parent unit server.")
    }

    if(!instance_desc->hasKey("control_unit_implementation")) {
        LOG_AND_TROW(CUCUI_ERR, -6, "The control unit implementaiton is mandatory within the instance description")
    }
        //
    if((err = cu_da->setInstanceDescription(cu_uid, *instance_desc.get()))) {
        LOG_AND_TROW(CUCUI_ERR, err, boost::str(boost::format("Error creating control unit instance description for node:%1%") % cu_uid));
    }
    
    //for compativbility  update here the default porperty values
    PropertyGroup pg(chaos::ControlUnitPropertyKey::P_GROUP_NAME);
    if(instance_desc->hasKey(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY)) {
        pg.addProperty(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY, instance_desc->getVariantValue(ControlUnitDatapackSystemKey::THREAD_SCHEDULE_DELAY));
    }
    if(instance_desc->hasKey(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE)) {
        pg.addProperty(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE, instance_desc->getVariantValue(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE));
    }
    if(instance_desc->hasKey(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_AGEING)) {
        pg.addProperty(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_AGEING, instance_desc->getVariantValue(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_AGEING));
    }
    if(instance_desc->hasKey(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME)) {
        pg.addProperty(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME, instance_desc->getVariantValue(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME));
    }
    if(instance_desc->hasKey(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME)) {
        pg.addProperty(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME, instance_desc->getVariantValue(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME));
    }
    if(instance_desc->hasKey(chaos::ControlUnitPropertyKey::INIT_RESTORE_OPTION)) {
        pg.addProperty(chaos::ControlUnitPropertyKey::INIT_RESTORE_OPTION, instance_desc->getVariantValue(chaos::ControlUnitPropertyKey::INIT_RESTORE_OPTION));
    }
    if(instance_desc->hasKey(chaos::ControlUnitPropertyKey::INIT_RESTORE_APPLY)) {
        pg.addProperty(chaos::ControlUnitPropertyKey::INIT_RESTORE_APPLY, instance_desc->getVariantValue(chaos::ControlUnitPropertyKey::INIT_RESTORE_APPLY));
    }
    if(instance_desc->hasKey(ControlUnitDatapackSystemKey::BYPASS_STATE)) {
        pg.addProperty(ControlUnitDatapackSystemKey::BYPASS_STATE, instance_desc->getVariantValue(ControlUnitDatapackSystemKey::BYPASS_STATE));
    }
    PropertyGroupVectorSDWrapper pgv_sdw;
    pgv_sdw().push_back(pg);
    if((err = n_da->updatePropertyDefaultValue(cu_uid, pgv_sdw()))){
        LOG_AND_TROW(CUCUI_ERR, err, CHAOS_FORMAT("Error updating property defaults for node:%1%",%cu_uid));
    }
    return CDWUniquePtr();
}
