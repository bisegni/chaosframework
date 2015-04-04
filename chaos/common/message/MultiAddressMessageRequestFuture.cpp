/*
 *	MultiAddressMessageRequestFuture.cpp
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

#include <chaos/common/message/MultiAddressMessageChannel.h>
#include <chaos/common/message/MultiAddressMessageRequestFuture.h>

#include <boost/format.hpp>

#define ERROR_MESS_DOMAIN_IN_STR(e,m,d)\
boost::str(boost::format("%1%-%2%(%3%)") % e % m % d)

#define MAMRF_APP INFO_LOG(MultiAddressMessageRequestFuture)
#define MAMRF_DBG DBG_LOG(MultiAddressMessageRequestFuture)
#define MAMRF_ERR ERR_LOG(MultiAddressMessageRequestFuture)

using namespace chaos::common::message;
    //!private destructor
MultiAddressMessageRequestFuture::MultiAddressMessageRequestFuture(chaos::common::message::MultiAddressMessageChannel *_parent_mn_message_channel,
                                                                   const std::string& _action_domain,
                                                                   const std::string& _action_name,
                                                                   chaos::common::data::CDataWrapper *_message_pack,
                                                                   int32_t _timeout_in_milliseconds):
timeout_in_milliseconds(_timeout_in_milliseconds),
parent_mn_message_channel(_parent_mn_message_channel),
action_domain(_action_domain),
action_name(_action_name),
message_pack(_message_pack){
    CHAOS_ASSERT(parent_mn_message_channel);
        //send data
    current_future = parent_mn_message_channel->_sendRequestWithFuture(action_domain,
                                                                       action_name,
                                                                       message_pack.get(),
                                                                       last_used_address);
}

    //!public destructor
MultiAddressMessageRequestFuture::~MultiAddressMessageRequestFuture() {

}

void MultiAddressMessageRequestFuture::resetErrorResult() {
    current_error = 0;
    current_error_message.clear();
    current_error_domain.clear();
}


void MultiAddressMessageRequestFuture::setTimeout(int32_t _timeout_in_milliseconds) {
    timeout_in_milliseconds = _timeout_in_milliseconds;
}

    //! wait until data is received
bool MultiAddressMessageRequestFuture::wait() {
    CHAOS_ASSERT(parent_mn_message_channel &&
                 current_future.get())
    bool working = true;
    
        //reset error
    resetErrorResult();

        //unitle we have valid future and don't have have answer
    while(current_future.get() &&
          working) {
        MAMRF_DBG << "Waiting on server " << last_used_address;
            //! waith for future
        if(current_future->wait(timeout_in_milliseconds)) {
            if(current_future->isRemoteMeaning()) {
                    //we have received from remote server somenthing
                working = false;
            }
        }

        if(working){
            MAMRF_DBG << "Error "<< ERROR_MESS_DOMAIN_IN_STR(current_future->getError(),
                                                             current_future->getErrorMessage(),
                                                             current_future->getErrorDomain())<<" during forward on " << last_used_address;
                //put offline current remote server

            parent_mn_message_channel->service_feeder.setURLOffline(parent_mn_message_channel->map_url_node_id[last_used_address].feeder_index);
            MAMRF_DBG << "Server " << last_used_address << " put offline";

            current_future = parent_mn_message_channel->_sendRequestWithFuture(action_domain,
                                                                               action_name,
                                                                               message_pack.get(),
                                                                               last_used_address);
            if(current_future.get()) {
                MAMRF_DBG << "Retransmission on " << last_used_address;
            }
        }
    }

    return working == false;
}

    //! try to get the result waiting for a determinate period of time
chaos::common::data::CDataWrapper *MultiAddressMessageRequestFuture::getResult() {
    if(!current_future.get()) return NULL;
    return current_future->getResult();
}


chaos::common::data::CDataWrapper *MultiAddressMessageRequestFuture::detachResult() {
    if(!current_future.get()) return NULL;
    return current_future->detachResult();
}

int MultiAddressMessageRequestFuture::getError() const {
    return current_error;
}

const std::string& MultiAddressMessageRequestFuture::getErrorDomain() const {
    return current_error_domain;
}

const std::string& MultiAddressMessageRequestFuture::getErrorMessage() const {
    return current_error_message;
}