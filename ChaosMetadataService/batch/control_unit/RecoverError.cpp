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

#include "RecoverError.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::control_unit;

#define CU_RE_BC_INFO INFO_LOG(RecoverError)
#define CU_RE_BC_DBG  DBG_LOG(RecoverError)
#define CU_RE_BC_ERR  ERR_LOG(RecoverError)

DEFINE_MDS_COMAMND_ALIAS(RecoverError)

RecoverError::RecoverError():
MDSBatchCommand(),
retry_number(0) {}

RecoverError::~RecoverError() {}

// inherited method
void RecoverError::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    CHECK_CDW_THROW_AND_LOG(data, CU_RE_BC_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID , CU_RE_BC_ERR, -2, "The ndk_uid key is mandatory")
    if(!data->isVectorValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) throw CException(-3, "ndk_uid key need to be a vectoro of string", __PRETTY_FUNCTION__);
    
    std::string uid;
    int         err         = 0;
    bool        presence    = false;
    
    //set cu id to the batch command datapack
    cu_ids = data->getVectorValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    for(int idx  = 0; idx < cu_ids->size(); idx++) {
        uid = cu_ids->getStringElementAtIndex(idx);
        //get control unit information and create the request
        if((err = getDataAccess<mds_data_access::ControlUnitDataAccess>()->checkPresence(uid, presence))){
            CU_RE_BC_ERR << "Error checking the presence of control unit "<< uid << " with error:" << err;
        } else if(presence) {
            CDataWrapper *tmp_pointer = NULL;
            if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(uid, &tmp_pointer))){
                
            } else {
                ChaosUniquePtr<chaos::common::data::CDataWrapper> cu_desc(tmp_pointer);
                //set the phase to send message
                
                requests.push_back(ChaosSharedPtr<RequestInfo>(createRequest(cu_desc->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
                                                                                cu_desc->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN),
                                                                                chaos::NodeDomainAndActionRPC::ACTION_NODE_RECOVER).release()));
  
            }
        }
    }
}

// inherited method
void RecoverError::acquireHandler() {
    MDSBatchCommand::acquireHandler();
}

// inherited method
void RecoverError::ccHandler() {
    MDSBatchCommand::ccHandler();
    
    for(std::vector< ChaosSharedPtr<RequestInfo> >::iterator it = requests.begin();
        it != requests.end();
        it++ ) {
        //send message without waiting for replay
        sendMessage(*(*it), NULL);
    }
    BC_END_RUNNING_PROPERTY
}

// inherited method
bool RecoverError::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}
