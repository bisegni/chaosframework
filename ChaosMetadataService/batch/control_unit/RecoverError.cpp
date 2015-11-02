/*
 *	RecoverError.cpp
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
retry_number(0) {
    //set default scheduler delay 1 second
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
    //set the timeout to 10 seconds
    setFeatures(common::batch_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)10000000);
}

RecoverError::~RecoverError() {}

// inherited method
void RecoverError::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    CHECK_CDW_THROW_AND_LOG(data, CU_RE_BC_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(data, chaos::NodeDefinitionKey::NODE_UNIQUE_ID , CU_RE_BC_ERR, -2, "The ndk_uid key is mandatory")
    if(data->isVectorValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) throw CException(-3, "ndk_uid key need to be a vectoro of string", __PRETTY_FUNCTION__);
    
    std::string uid;
    int         err         = 0;
    bool        presence    = false;
    
    //set cu id to the batch command datapack
    cu_ids.reset(data->getVectorValue(NodeDefinitionKey::NODE_UNIQUE_ID));
    
    for(int idx  = 0; idx < cu_ids->size(); idx++) {
        uid = cu_ids->getStringElementAtIndex(idx);
        //get control unit information and create the request
        if((err = getDataAccess<mds_data_access::ControlUnitDataAccess>()->checkPresence(uid, presence))){
            CU_RE_BC_ERR << "Error checking the presence of control unit "<< uid << " with error:" << err;
        } else if(presence) {
            CDataWrapper *tmp_pointer = NULL;
            if((err = getDataAccess<mds_data_access::NodeDataAccess>()->getNodeDescription(uid, &tmp_pointer))){
                
            } else {
                std::auto_ptr<CDataWrapper> cu_desc(tmp_pointer);
                //set the phase to send message
                
                requests.push_back(boost::shared_ptr<RequestInfo>(createRequest(cu_desc->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR),
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
    
    for(std::vector< boost::shared_ptr<RequestInfo> >::iterator it = requests.begin();
        it != requests.end();
        it++ ) {
        //send message without waiting for replay
        sendMessage(*(*it), NULL);
    }
}

// inherited method
bool RecoverError::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    return result;
}