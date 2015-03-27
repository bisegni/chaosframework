//
//  GetNodeDescription.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 27/03/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#include "GetNodeDescription.h"
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

//! default constructor
GetNodeDescription::GetNodeDescription(chaos::common::message::MultiAddressMessageChannel *_mn_message):
ApiProxy("getNodeDescription", _mn_message) {
    
}
//! default destructor
GetNodeDescription::~GetNodeDescription() {
    
}

/*!

 */
ApiProxyResult GetNodeDescription::execute(const std::string& unique_node_id) {
    chaos::common::data::CDataWrapper message;
    message.addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, unique_node_id);
    return callApi("system", getName(), &message);
}