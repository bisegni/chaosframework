/*
 *	NodeRegister.cpp
 *	!CHAOS
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
#include "../../batch/control_unit/RegistrationAckBatchCommand.h"
#include "../../batch/agent/AgentAckCommand.h"

#include <boost/algorithm/string/predicate.hpp>

#define USRA_INFO INFO_LOG(NodeRegister)
#define USRA_DBG  DBG_LOG(NodeRegister)
#define USRA_ERR  ERR_LOG(NodeRegister)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::node;
using namespace chaos::metadata_service::persistence::data_access;

NodeRegister::NodeRegister():
AbstractApi(chaos::MetadataServerNodeDefinitionKeyRPC::ACTION_REGISTER_NODE){}

NodeRegister::~NodeRegister() {}

CDataWrapper *NodeRegister::execute(CDataWrapper *api_data,
                                    bool& detach_data) throw (chaos::CException){
    CHECK_CDW_THROW_AND_LOG(api_data, USRA_ERR, -1, "No parameter found")
    
    CDataWrapper *result = NULL;
    if(!api_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
        throw CException(-1, "Node unique id not found", __PRETTY_FUNCTION__);
    }
    
    if(!api_data->hasKey(NodeDefinitionKey::NODE_TYPE)) {
        throw CException(-2, "Node type not found", __PRETTY_FUNCTION__);
    }
    
    if(!api_data->hasKey(NodeDefinitionKey::NODE_RPC_ADDR)) {
        throw CException(-2, "RPC address not found", __PRETTY_FUNCTION__);
    }
    
    const std::string node_type = api_data->getStringValue(NodeDefinitionKey::NODE_TYPE);
    if(node_type.compare(NodeType::NODE_TYPE_UNIT_SERVER) == 0) {
        result = unitServerRegistration(api_data,
                                        detach_data);
    } else if(boost::starts_with(node_type, NodeType::NODE_TYPE_CONTROL_UNIT)) {
        result = controlUnitRegistration(api_data,
                                         detach_data);
    } else if(boost::starts_with(node_type, NodeType::NODE_TYPE_AGENT)) {
        result = agentRegistration(api_data,
                                   detach_data);
    } else {
        throw CException(-3, "Type of node not managed for registration", __PRETTY_FUNCTION__);
    }
    
    return result;
}

CDataWrapper *NodeRegister::agentRegistration(CDataWrapper *api_data,
                                              bool& detach_data) throw(chaos::CException) {
    
    int err = 0;
    uint64_t    command_id;
    //data is used to send answer so we tag it to get ownership
    detach_data = true;
    const std::string agent_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    //fetch the unit server data access
    GET_DATA_ACCESS(AgentDataAccess, a_da, -1)
    try {
        if((err = a_da->insertUpdateAgentDescription(*api_data))) {
            LOG_AND_TROW(USRA_ERR, -1, CHAOS_FORMAT("Error %1% registering agent %2%", %err%agent_uid));
        }
        //now we can send back the received message with the ack result
        api_data->addInt32Value(AgentNodeDomainAndActionRPC::REGISTRATION_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_OK);
    } catch (...) {
        api_data->addInt32Value(AgentNodeDomainAndActionRPC::REGISTRATION_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::unit_server::UnitServerAckCommand),
                                          api_data);
        LOG_AND_TROW(USRA_ERR, -7, "Unknown exception")
    }
    
    //manage ack into back command
    command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::agent::AgentAckCommand),
                                                   api_data,
                                                   0,
                                                   1000);
    
    USRA_INFO << CHAOS_FORMAT("Sent ack for agent %1% registration", %agent_uid);
    return NULL;
}

CDataWrapper *NodeRegister::unitServerRegistration(CDataWrapper *api_data,
                                                   bool& detach_data) throw(chaos::CException) {
    
    int err = 0;
    uint64_t    command_id;
    bool        is_present = false;
    uint64_t    nodes_seq = 0;
    //data is used to send answer so we tag it to get ownership
    detach_data = true;
    
    //fetch the unit server data access
    GET_DATA_ACCESS(UnitServerDataAccess, us_da, -1)
    GET_DATA_ACCESS(UtilityDataAccess, u_da, -2)
    
    const std::string unit_server_alias = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    USRA_INFO << "Register unit server " << unit_server_alias;
    
    try {
        if((err = us_da->checkPresence(unit_server_alias, is_present))) {
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
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::unit_server::UnitServerAckCommand),
                                                       api_data);
        USRA_ERR << "Sent ack for registration denied to the unit server " << unit_server_alias;
        throw ex;
    } catch (...) {
        api_data->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::unit_server::UnitServerAckCommand),
                                          api_data);
        LOG_AND_TROW(USRA_ERR, -7, "Unknown exception")
        USRA_ERR << "Sent ack for registration denied to the unit server " << unit_server_alias;
    }
    
    //all is gone weel
    command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::unit_server::UnitServerAckCommand),
                                                   api_data,
                                                   0,
                                                   1000);
    
    USRA_INFO << "Sent ack for registration ok to the unit server " << unit_server_alias;
    return NULL;
}

//! perform specific registration for control unit
CDataWrapper *NodeRegister::controlUnitRegistration(CDataWrapper *api_data,
                                                    bool& detach_data) throw(chaos::CException) {
    int         err = 0;
    uint64_t    command_id;
    std::string us_host;
    bool        is_present = false;
    bool        loaded_from_unit_server = false;
    bool        has_an_unit_server = false;
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_RPC_DOMAIN, USRA_ERR, -1, "The rpc domain key is mandatory")
    CHECK_KEY_THROW_AND_LOG(api_data, ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION, USRA_ERR, -2, "The cudk_ds_desc key is mandatory")
    
    //fetch the unit server data access
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -3)
    GET_DATA_ACCESS(NodeDataAccess, n_da, -4)
    GET_DATA_ACCESS(UnitServerDataAccess, us_da, -5)
    
    //allocate datapack for batch command
    std::unique_ptr<CDataWrapper> ack_command(new CDataWrapper());
    const std::string cu_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    USRA_INFO << "Register control unit " << cu_uid;
    
    //set cu id to the batch command datapack
    ack_command->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID,
                                cu_uid);
    //aset rpc address
    ack_command->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR,
                                api_data->getStringValue(NodeDefinitionKey::NODE_RPC_ADDR));
    //aset rpc address
    ack_command->addStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN,
                                api_data->getStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN));
    
    try {
        //check if the cu has been loaded from unit server
        if(api_data->hasKey("mds_control_key")) {
            const std::string mds_ctrl_key = api_data->getStringValue("mds_control_key");
            loaded_from_unit_server = (mds_ctrl_key.compare("mds") == 0);
        }
        
        //check if the cu has a parent
        //we need to check if the control unit is assocaite to an unit server
        if((err = us_da->getUnitserverForControlUnitID(cu_uid,
                                                       us_host))){
            LOG_AND_TROW_FORMATTED(USRA_ERR, -6, "Error searching unit server for control unit %1% with code %2%",%cu_uid%err);
        }
        has_an_unit_server = (us_host.size()>0);
        
        if(has_an_unit_server) {
            //@TODO removed this check that is wrong for WAN dataset registration
            //  if(loaded_from_unit_server == false) {LOG_AND_TROW_FORMATTED(USRA_ERR, -7, "The control unit %1% need to be loaded from the unit server %2%",%cu_uid%us_host);}
        }
        
        //check if the node is present
        if((err = cu_da->checkPresence(cu_uid, is_present))) {
            LOG_AND_TROW(USRA_ERR, -8, "error checking the control unit presence");
        } if (is_present) {
            if((err = n_da->updateNode(*api_data))) {
                LOG_AND_TROW(USRA_ERR, err, "Error updating default node field");
            }
            
            if((err = cu_da->setDataset(cu_uid,
                                        *api_data))){
                LOG_AND_TROW(USRA_ERR, err, "error setting the dataset of the control unit");
            }
            //ok->registered
            ack_command->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                       ErrorCode::EC_MDS_NODE_REGISTRATION_OK);
        } else {
            
            USRA_INFO << "Control unit " << cu_uid << " not present, check if it is hosted by an unit server";
            //we need to check if the control unit is assocaite to an unit server
            if((err = us_da->getUnitserverForControlUnitID(cu_uid,
                                                           us_host) == 0)){
                if(us_host.size() == 0) {
                    USRA_INFO << "Control unit " << cu_uid << " doesn't belong to any unit server so can ste forwardar";
                    
                    if((err = cu_da->insertNewControlUnit(*api_data))) {
                        LOG_AND_TROW_FORMATTED(USRA_ERR, err, "Error creating new control unit %1%",%cu_uid);
                    }
                    
                    if((err = cu_da->setDataset(cu_uid,
                                                *api_data))){
                        LOG_AND_TROW_FORMATTED(USRA_ERR, err, "error setting the dataset for the control unit %1%",%cu_uid);
                    }
                }else {
                    if(loaded_from_unit_server == false) {
                        //need to be stoped becaus the cu need be loaded form unit server
                        USRA_ERR << "The control unit "<< cu_uid <<" need to be stoped because the cu need be loaded form unit server";
                        //actiually only the control unit hosted by unit server can be registered
                        ack_command->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                                   ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
                    }
                }
            } else {
                LOG_AND_TROW_FORMATTED(USRA_ERR, -9, "Error searchin unit server for control unit %1% with code %2%",%cu_uid%err);
            }
            
        }
        
        //set the code to inform cu that all is gone well
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::control_unit::RegistrationAckBatchCommand),
                                                       ack_command.release(),
                                                       0,
                                                       1000);
        USRA_INFO << "Sent ack for registration ok to the control unit " << cu_uid << " with commadn id:" <<command_id;
    } catch (chaos::CException& ex) {
        ack_command->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                   ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        
        USRA_ERR << "Sent ack for registration denied to the unit server " << cu_uid;
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::control_unit::RegistrationAckBatchCommand),
                                                       ack_command.release());
        throw ex;
    } catch (...) {
        USRA_ERR << "Sent ack for registration denied to the unit server " << cu_uid;
        //somenthing goes worng so deny the registration
        ack_command->addInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT,
                                   ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS);
        command_id = getBatchExecutor()->submitCommand(GET_MDS_COMMAND_ALIAS(batch::control_unit::RegistrationAckBatchCommand),
                                                       ack_command.release());
        LOG_AND_TROW(USRA_ERR, -6, "Unknown exception")
    }
    return NULL;
}
