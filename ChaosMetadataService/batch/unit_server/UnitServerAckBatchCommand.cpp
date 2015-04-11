/*
 *	UnitServerAckCommand.cpp
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
#include "UnitServerAckBatchCommand.h"

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::metadata_service::batch::unit_server;

#define USAC_INFO INFO_LOG(UnitServerAckCommand)
#define USAC_DBG  DBG_LOG(UnitServerAckCommand)
#define USAC_ERR  ERR_LOG(UnitServerAckCommand)

DEFINE_MDS_COMAMND_ALIAS(UnitServerAckCommand)

static const char * const NET_ADDR_ALLOC_ERR = "CNetworkAddress allocation error";
static const char * const MESS_CHNL_ALLO_ERR = "Message channel allocation error";

UnitServerAckCommand::UnitServerAckCommand():
MDSBatchCommand(),
retry_number(0),
message_channel(NULL),
message_data(NULL){
    
}
UnitServerAckCommand::~UnitServerAckCommand() {
    if(message_channel) {
        USAC_INFO << "Release message channel";
        releaseChannel(message_channel);
    }
}

// inherited method
void UnitServerAckCommand::setHandler(CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    if(data  && data->hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) {
        remote_unitserver_address = new CNetworkAddress();
        if(remote_unitserver_address) {
            remote_unitserver_address->ip_port = data->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR);
            USAC_INFO << "fetch the message channel for:"<<remote_unitserver_address->ip_port;
            //delete(na);
            message_channel = getNewMessageChannel();
            if(!message_channel) {
                USAC_ERR << MESS_CHNL_ALLO_ERR;
                throw chaos::CException(-1, MESS_CHNL_ALLO_ERR, __PRETTY_FUNCTION__);
            }
        } else {
            USAC_ERR << NET_ADDR_ALLOC_ERR;
            throw chaos::CException(-2, NET_ADDR_ALLOC_ERR, __PRETTY_FUNCTION__);
        }
        
        message_data = data;
    }

}

// inherited method
void UnitServerAckCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
    USAC_INFO << "execute acquire handler";
}

// inherited method
void UnitServerAckCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    auto_ptr<CDataWrapper> result;
    USAC_INFO << "execute ccHandler";
    retry_number++;
    result.reset(message_channel->sendRequest(remote_unitserver_address->ip_port,
                                              "system",
                                              UnitServerNodeDomainAndActionLabel::ACTION_UNIT_SERVER_REG_ACK,
                                              message_data,
                                              1000));
    if(message_channel->getLastErrorCode() != 0) {
        USAC_INFO << "error sending message to the unit server " << remote_unitserver_address->ip_port;
        if(retry_number > 3) {
            USAC_INFO << "We have exeeced the number of retry this is the last retry for unit server act to " << remote_unitserver_address->ip_port;
            BC_END_RUNNIG_PROPERTY;
        }
    } else {
        BC_END_RUNNIG_PROPERTY;
    }
}

// inherited method
bool UnitServerAckCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    USAC_INFO << "execute ccHandler with " << result;
    return result;
}