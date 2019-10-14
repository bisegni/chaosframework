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

#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <chaos/common/message/MultiAddressMessageRequestFuture.h>

#include <boost/format.hpp>

#define ERROR_MESS_DOMAIN_IN_STR(e, m, d) \
boost::str(boost::format("%1%-%2%(%3%)") % e % m % d)

#define MAMRF_INFO INFO_LOG(MultiAddressMessageRequestFuture)
#define MAMRF_DBG DBG_LOG(MultiAddressMessageRequestFuture)
#define MAMRF_ERR ERR_LOG(MultiAddressMessageRequestFuture)

using namespace chaos::common::data;
using namespace chaos::common::message;
//!private destructor
#define CHECK_NULL_MESSAGE(message_pack) (message_pack.get() ? MOVE(message_pack->clone()) : CDWUniquePtr())

MultiAddressMessageRequestFuture::MultiAddressMessageRequestFuture(chaos::common::message::MultiAddressMessageChannel* _parent_mn_message_channel,
                                                                   const std::string&                                  _action_domain,
                                                                   const std::string&                                  _action_name,
                                                                   CDWUniquePtr                                        _message_pack,
                                                                   int32_t                                             _timeout_in_milliseconds):
timeout_in_milliseconds(_timeout_in_milliseconds),
parent_mn_message_channel(_parent_mn_message_channel),
action_domain(_action_domain),
action_name(_action_name),
message_pack(MOVE(_message_pack)) {
    CHAOS_ASSERT(parent_mn_message_channel);
    //send data
    current_future = parent_mn_message_channel->_sendRequestWithFuture(action_domain,
                                                                       action_name,
                                                                       CHECK_NULL_MESSAGE(message_pack),
                                                                       last_used_address);
}

//!public destructor
MultiAddressMessageRequestFuture::~MultiAddressMessageRequestFuture() {}

void MultiAddressMessageRequestFuture::setTimeout(int32_t _timeout_in_milliseconds) {
    timeout_in_milliseconds = _timeout_in_milliseconds;
}

//! wait until data is received
bool MultiAddressMessageRequestFuture::wait() {
    CHAOS_ASSERT(parent_mn_message_channel)
    int  retry_on_same_server = 0;
    int  retry_other_server   = 0;
    bool working              = true;
    //unitl we have valid future and don't have have answer
    
    while (retry_other_server < 2 &&
           working) {
        while (current_future.get() &&
               working) {
            MAMRF_DBG << "Waiting on server " << last_used_address << " for " << timeout_in_milliseconds << " ms";
            //! waith for future
            
            if (current_future->wait(timeout_in_milliseconds)) {
                MAMRF_DBG << "Exit Waiting on server " << last_used_address << " for " << timeout_in_milliseconds << " ms got:" << current_future->isRemoteMeaning();
                if (current_future->isRemoteMeaning()) {
                    //we have received from remote server somenthing
                    working = false;
                } else {
                    //we have submission error
                    if (current_future->getError()) {
                        MAMRF_ERR << "We have submission error:" << current_future->getError() << " message:" << current_future->getErrorMessage() << " domain:" << current_future->getErrorDomain();
                        
                        //set current server offline
                        parent_mn_message_channel->setURLAsOffline(last_used_address);
                        // working = false;
                    }
                }
            } else {
                if (retry_on_same_server++ < 3) {
                    MAMRF_INFO << "Retry to wait on same server for " << timeout_in_milliseconds;
                    continue;
                } else {
                    MAMRF_INFO << "We have retried " << retry_on_same_server << " times on " << last_used_address;
                    //switchOnOtherServer();
                    parent_mn_message_channel->setURLAsOffline(last_used_address);
                    // working = false;
                }
            }
        }
        
        if(working) {
            //we need to try to send request to another server
            current_future = parent_mn_message_channel->_sendRequestWithFuture(action_domain,
                                                                               action_name,
                                                                               CHECK_NULL_MESSAGE(message_pack),
                                                                               last_used_address);
            if(current_future.get() == NULL) {
                MAMRF_ERR << "Retramission has not ben possible, no other server are available";
            }
        }
    }
    return working == false;
}

//! try to get the result waiting for a determinate period of time
CDataWrapper* MultiAddressMessageRequestFuture::getResult() {
    if (current_future.get())
        return current_future->getResult();
    else
        return NULL;
}

CDWUniquePtr MultiAddressMessageRequestFuture::detachResult() {
    if (current_future.get())
        return current_future->detachResult();
    else
        return CDWUniquePtr();
}

int MultiAddressMessageRequestFuture::getError() const {
    if (current_future.get())
        return current_future->getError();
    else
        return ErrorRpcCoce::EC_RPC_REQUEST_FUTURE_NOT_AVAILABLE;
}

const std::string& MultiAddressMessageRequestFuture::getErrorDomain() const {
    if (current_future.get())
        return current_future->getErrorDomain();
    else
        return ErrorRpcCoce::EC_RPC_ERROR_DOMAIN;
}

const std::string& MultiAddressMessageRequestFuture::getErrorMessage() const {
    if (current_future.get())
        return current_future->getErrorMessage();
    else
        return ErrorRpcCoce::EC_REQUEST_FUTURE_NOT_AVAILABLE;
}
