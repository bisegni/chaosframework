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
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CUCUI_INFO INFO_LOG(SetInstanceDescription)
#define CUCUI_DBG  DBG_LOG(SetInstanceDescription)
#define CUCUI_ERR  ERR_LOG(SetInstanceDescription)

SetInstanceDescription::SetInstanceDescription():
AbstractApi("setInstanceDescription"){

}

SetInstanceDescription::~SetInstanceDescription() {

}

chaos::common::data::CDataWrapper *SetInstanceDescription::execute(chaos::common::data::CDataWrapper *api_data,
                                                                   bool& detach_data) throw(chaos::CException) {

    int err = 0;
    bool presence = false;
    if(!api_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        LOG_AND_TROW(CUCUI_ERR, -1, "Node unique id is mandatory")
    }
    if(!api_data->hasKey("instance_description")) {
        LOG_AND_TROW(CUCUI_ERR, -2, "the instance description need to be associated to a the key 'instance_description'");
    }

        //!get the unit server data access
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -3)
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4)
    if((err = n_da->checkNodePresence(presence,
                                      api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID)))){
        LOG_AND_TROW(CUCUI_ERR, err, "Error checking node presence")
    }
    if (!presence) {
        ChaosUniquePtr<chaos::common::data::CDataWrapper> node_min_dec(new CDataWrapper());
        node_min_dec->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID));
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
    if((err = cu_da->setInstanceDescription(api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID), *instance_desc.get()))) {
        LOG_AND_TROW(CUCUI_ERR, err, boost::str(boost::format("Error creating control unit instance description for node:%1%") % api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID)))
    }
    return NULL;
}
