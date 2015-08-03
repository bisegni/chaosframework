/*
 *  BatchCommand.cpp
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
#include "MDSBatchCommand.h"

using namespace chaos::common::network;
using namespace chaos::metadata_service::batch;
using namespace chaos::common::batch_command;
#define MDSBC_INFO INFO_LOG(MDSBatchCommand)
#define MDSBC_DBG  DBG_LOG(MDSBatchCommand)
#define MDSBC_ERR  ERR_LOG(MDSBatchCommand)

//! default constructor
MDSBatchCommand::MDSBatchCommand():
BatchCommand(){
    
}

//! default destructor
MDSBatchCommand::~MDSBatchCommand() {
    
}

chaos::common::message::MessageChannel*
MDSBatchCommand::getMessageChannel() {
    CHAOS_ASSERT(message_channel)
    return message_channel;
}

chaos::common::message::MultiAddressMessageChannel*
MDSBatchCommand::getMultiAddressMessageChannel() {
    CHAOS_ASSERT(multiaddress_message_channel)
    return multiaddress_message_channel;
}

// return the implemented handler
uint8_t MDSBatchCommand::implementedHandler() {
    return  HandlerType::HT_Set|
    HandlerType::HT_Correlation|
    HandlerType::HT_Acquisition;
}

// inherited method
void MDSBatchCommand::setHandler(chaos_data::CDataWrapper *data) {
    BC_EXEC_RUNNIG_PROPERTY
}

// inherited method
void MDSBatchCommand::acquireHandler() {
    
}

// inherited method
void MDSBatchCommand::ccHandler() {
    
}

// inherited method
bool MDSBatchCommand::timeoutHandler() {
    return true;
}

//! create a request to a remote rpc action
std::auto_ptr<RequestInfo> MDSBatchCommand::createRequest(const std::string& remote_address,
                                         const std::string& remote_domain,
                                         const std::string& remote_action) throw (chaos::CException) {
    return std::auto_ptr<RequestInfo> (new RequestInfo(remote_address,
                                                       remote_domain,
                                                       remote_action));
}

void MDSBatchCommand::sendRequest(RequestInfo& request_info,
                                  chaos::common::data::CDataWrapper *message) throw (chaos::CException) {
    CHAOS_ASSERT(message_channel)
    
    request_info.request_future = message_channel->sendRequestWithFuture(request_info.remote_address,
                                                                         request_info.remote_domain,
                                                                         request_info.remote_action,
                                                                         message);
    request_info.phase = MESSAGE_PHASE_SENT;
}

void MDSBatchCommand::manageRequestPhase(RequestInfo& request_info) throw (chaos::CException) {
    switch(request_info.phase) {
        case MESSAGE_PHASE_UNSENT:
            throw chaos::CException(-1, "Request is unsent", __PRETTY_FUNCTION__);
            break;
        case MESSAGE_PHASE_SENT: {
            MDSBC_DBG << "Waith for "<< request_info.remote_action <<
            " action ack from the control unit:" << request_info.remote_address <<
            " on rpc[" << request_info.remote_domain <<":"<<request_info.remote_action<<"]";
            if(!request_info.request_future.get()) {
                MDSBC_ERR << "request with no future";
                throw chaos::CException(-2, "request with no future", __PRETTY_FUNCTION__);
            }
            if(request_info.request_future->wait(5000)) {
                //we have hd answer
                if(request_info.request_future->getError()) {
                    MDSBC_ERR << "We have had answer with error"
                    << request_info.request_future->getError() << " \n and message "
                    << request_info.request_future->getErrorMessage() << "\n on domain "
                    << request_info.request_future->getErrorDomain();
                } else {
                    MDSBC_DBG << "Request send with success";
                }
                request_info.phase = MESSAGE_PHASE_COMPLETED;
            } else {
                if((request_info.retry % 3) == 0) {
                    request_info.phase = MESSAGE_PHASE_TIMEOUT;
                }
            }
            break;
        }
        case MESSAGE_PHASE_COMPLETED:
        case MESSAGE_PHASE_TIMEOUT:
            break;
    }
}