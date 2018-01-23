 /*
 *	MessageRequestFuture.cpp
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
#include <chaos/common/message/MessageRequestFuture.h>

#define MRF_INFO INFO_LOG(MessageRequestFuture)
#define MRF_DBG DBG_LOG(MessageRequestFuture)
#define MRF_ERR ERR_LOG(MessageRequestFuture)

using namespace chaos::common::data;
using namespace chaos::common::message;

//!private constructor
MessageRequestFuture::MessageRequestFuture(chaos::common::utility::atomic_int_type _request_id,
                                           MessageRequestDomainFutureHelper::Future& _message_future):
request_id(_request_id),
message_future(ChaosMoveOperator(_message_future)),
request_result(),
error_code(0),
error_message("no data"),
error_domain("no domain"),
local_result(false) {}

MessageRequestFuture::~MessageRequestFuture() {}

bool MessageRequestFuture::wait(int32_t timeout_in_milliseconds) {
    bool result = false;
    ChaosFutureStatus fret = ChaosFutureStatus::deferred;
    try{
        if(request_result.get()) {
            result = true;
        } else{
            //! whait for result
            if (timeout_in_milliseconds >= 0){
                MRF_DBG<<CHAOS_FORMAT("Future wait for %1% milliseconds",%timeout_in_milliseconds);
                fret=message_future.wait_for(ChaosCronoMilliseconds(timeout_in_milliseconds));
            } else {
                message_future.wait();
            }
            if (fret == ChaosFutureStatus::ready){
                CDWShrdPtr message_data = message_future.get();
                DEBUG_CODE(MRF_DBG << message_data->getJSONString();)
                MRF_PARSE_CDWPTR_RESULT(message_data.get())
                result = true;
            }
        }
    } catch (boost::broken_promise &e) {
        MRF_ERR << CHAOS_FORMAT("Broken promises error:%1%",%e.what());
    } catch (...) {
        MRF_ERR << "Broken promises unrecognied error";
    }
    return result;
}

//! try to get the result waiting for a determinate period of time
chaos::common::data::CDataWrapper *MessageRequestFuture::getResult() {
    //! wait for result
    return request_result.get();
}

chaos::common::data::CDataWrapper *MessageRequestFuture::detachResult() {
    return request_result.release();
}

uint32_t const & MessageRequestFuture::getRequestID() {
    return request_id;
}

int MessageRequestFuture::getError() const {
    return error_code;
}

const std::string& MessageRequestFuture::getErrorDomain() const {
    return error_domain;
}

const std::string& MessageRequestFuture::getErrorMessage() const {
    return error_message;
}

bool MessageRequestFuture::isRemoteMeaning() {
    return !local_result;
}
