/*
 *	MessageRequestFuture.cpp
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
#include <chaos/common/message/MessageRequestFuture.h>

using namespace chaos::common::message;

    //!private constructor
MessageRequestFuture::MessageRequestFuture(chaos::common::utility::atomic_int_type _request_id,
                                           boost::unique_future<chaos::common::data::CDataWrapper*> _future):
request_id(_request_id),
future(_future),
request_result(NULL),
error_code(0),
error_message(""),
error_domain(""){

}
    //!private destructor
MessageRequestFuture::~MessageRequestFuture() {

}

bool MessageRequestFuture::wait(int32_t timeout_in_milliseconds) {
    if(request_result.get()) {
        return true;
    }
    //! whait for result
    if(timeout_in_milliseconds >= 0) {
        future.wait_for(boost::chrono::milliseconds(timeout_in_milliseconds));
    } else {
        future.wait();
    }
    
    return future.is_ready();
}

    //! try to get the result waiting for a determinate period of time
chaos::common::data::CDataWrapper *MessageRequestFuture::getResult() {
        //! see if we have already the result
    if(request_result.get()) {
        return request_result.get();
    }
        //! wait for result
    if(future.is_ready() &&
       future.has_value()) {
        MRF_PARSE_CDWPTR_RESULT(future.get())
        return request_result.get();
    } else {
        return NULL;
    }
}

chaos::common::data::CDataWrapper *MessageRequestFuture::detachResult() {
    return request_result.release();
}

const chaos::common::utility::atomic_int_type& MessageRequestFuture::getRequestID() {
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