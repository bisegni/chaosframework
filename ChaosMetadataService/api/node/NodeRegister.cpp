/*
 *	NodeRegister.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#include "NodeRegister.h"
#include "../../batch/unit_server/UnitServerAckBatchCommand.h"
#define USRA_INFO INFO_LOG(NodeRegister)
#define USRA_DBG  DBG_LOG(NodeRegister)
#define USRA_ERR  ERR_LOG(NodeRegister)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::node;

NodeRegister::NodeRegister():
AbstractApi(chaos::MetadataServerNodeDefinitionKeyRPC::ACTION_REGISTER_NODE){

}

NodeRegister::~NodeRegister() {

}

chaos::common::data::CDataWrapper *NodeRegister::execute(chaos::common::data::CDataWrapper *api_data,
                                                         bool& detach_data) throw (chaos::CException){
    CHAOS_ASSERT(api_data)
    USRA_INFO << api_data->getJSONString();

    chaos::common::data::CDataWrapper *result = NULL;
    if(!api_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        throw CException(-1, "Node unique id not found", __PRETTY_FUNCTION__);
    }

    if(!api_data->hasKey(NodeDefinitionKey::NODE_TYPE)) {
        throw CException(-2, "Node type not found", __PRETTY_FUNCTION__);
    }

    const std::string node_type = api_data->getStringValue(NodeDefinitionKey::NODE_TYPE);
    if(node_type.compare(NodeType::NODE_TYPE_UNIT_SERVER) == 0) {
            //perform unit server registration
        result = unitServerRegistration(api_data,
                                        detach_data);
    } else {
        throw CException(-3, "Bad unit server type forwarded", __PRETTY_FUNCTION__);
    }

    return result;
}

chaos::common::data::CDataWrapper *NodeRegister::unitServerRegistration(chaos::common::data::CDataWrapper *api_data,
                                                                        bool& detach_data) throw(chaos::CException) {

    int err = 0;
    uint64_t    command_id;
    bool        is_present = false;
    uint64_t    nodes_seq = 0;
    detach_data = true;

        //fetch the unit server data access
    GET_DATA_ACCESS(persistence::data_access::UnitServerDataAccess, us_da, -1)
    GET_DATA_ACCESS(persistence::data_access::UtilityDataAccess, u_da, -2)

    const std::string unit_server_alias = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    try {
        if((err = us_da->checkUSPresence(unit_server_alias, is_present))) {
                //err
            api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                    ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
            LOG_AND_TROW(USRA_ERR, -3, "error checking the unit server presence")
        }if(is_present) {
                //present
            if((err = us_da->updateUS(*api_data))) {
                api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                        ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
                LOG_AND_TROW(USRA_ERR, -4, "error updating the unit server information")
            }
        }else {
                //unit server not found, so we need to register new one
            if((err = u_da->getNextSequenceValue("nodes", nodes_seq))) {
                LOG_AND_TROW(USRA_ERR, err, "error fetching the new sequence value")
            }else{
                    //add new fetched sequecne
                api_data->addInt64Value("seq", nodes_seq);

                    //insert th enew node
                if((err = us_da->insertNewUS(*api_data))) {
                    api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                            ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
                    LOG_AND_TROW(USRA_ERR, -6, "error saving the new unit server information")
                }
            }
        }
            //now we can send back the received message with the ack result
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_OK);
    } catch (chaos::CException& ex) {
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ASLIAS(batch::unit_server::UnitServerAckCommand)),
                                          api_data,
                                          command_id);
        USRA_ERR << "Sent ack for registration denied to the unit server " << unit_server_alias;
        throw ex;
    } catch (...) {
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ASLIAS(batch::unit_server::UnitServerAckCommand)),
                                          api_data,
                                          command_id);
        LOG_AND_TROW(USRA_ERR, -7, "Unknown exception")
        USRA_ERR << "Sent ack for registration denied to the unit server " << unit_server_alias;
    }

        //all is gone weel
    USRA_DBG << "Send ack for registration ok to the unit server " << unit_server_alias;
    getBatchExecutor()->submitCommand(std::string(GET_MDS_COMMAND_ASLIAS(batch::unit_server::UnitServerAckCommand)),
                                      api_data,
                                      command_id);
    
    return NULL;
}