/*
 *	UnitServerAckCommand.cpp
 *	!CHOAS
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

using namespace chaos::metadata_service::batch::unit_server;
#define USAC_INFO INFO_LOG(UnitServerAckCommand)
#define USAC_DBG  INFO_DBG(UnitServerAckCommand)
#define USAC_ERR  INFO_ERR(UnitServerAckCommand)

DEFINE_MDS_COMAMND_ALIAS(UnitServerAckCommand)

// inherited method
void UnitServerAckCommand::setHandler(chaos_data::CDataWrapper *data) {
    MDSBatchCommand::setHandler(data);
    USAC_INFO << "execute set handler";
    //message_channel = network_broker->getRawMessageChannelFromAddress(<#chaos::common::network::CNetworkAddress *deviceNetworkAddress#>);
}

// inherited method
void UnitServerAckCommand::acquireHandler() {
    MDSBatchCommand::acquireHandler();
    USAC_INFO << "execute acquire handler";
}

// inherited method
void UnitServerAckCommand::ccHandler() {
    MDSBatchCommand::ccHandler();
    USAC_INFO << "execute ccHandler";
    BC_END_RUNNIG_PROPERTY;
}

// inherited method
bool UnitServerAckCommand::timeoutHandler() {
    bool result = MDSBatchCommand::timeoutHandler();
    USAC_INFO << "execute ccHandler with " << result;
    return result;
}