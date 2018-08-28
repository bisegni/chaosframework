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
#ifndef __CHAOSFramework__MessageRequestFuture__
#define __CHAOSFramework__MessageRequestFuture__
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/thread/FutureHelper.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/Atomic.h>

#include <boost/smart_ptr.hpp>
#include <boost/thread/future.hpp>

#define MRF_PARSE_CDWPTR_RESULT(x) \
if(x==NULL) {\
error_code = ErrorCode::EC_TIMEOUT;\
local_result = true;\
} else {\
local_result = x->hasKey("__internal_redirect__");\
if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE)) request_result.reset(x->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE).release());\
if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)) error_code = x->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);\
if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE)) error_message = x->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE);\
if(x->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN)) error_domain = x->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN);\
}

namespace chaos {
    namespace common{
        namespace message {
            class MessageRequestDomain;
            
            typedef chaos::common::thread::FutureHelper<chaos::common::data::CDWShrdPtr> MessageRequestDomainFutureHelper;
            
                //! class that mange the fetch of the result for a request
            /*!
             When a request is forwarded, thi class can be requested to whait or poll whene a result
             for a request is arrived.
             */
            class MessageRequestFuture {
                friend class MessageRequestDomain;
                chaos::common::utility::atomic_int_type request_id;

                    //shared future for the answer
                MessageRequestDomainFutureHelper::Future message_future;

                    //! the result for the request
                chaos::common::data::CDWUniquePtr request_result;
                
                    //!error code
                int32_t error_code;

                    //!last error message
                std::string error_message;

                    //!last error domain
                std::string error_domain;

                bool local_result;

                    //! set the result fo the request
                void setResult(chaos::common::data::CDataWrapper *_request_result);

                    //!private constructor
                MessageRequestFuture(chaos::common::utility::atomic_int_type _request_id,
                                     MessageRequestDomainFutureHelper::Future& _message_future);

            public:
                //!private destructor
                ~MessageRequestFuture();
                
                //! wait until data is received
                bool wait(int32_t timeout_in_milliseconds);
                
                    //! try to get the result waiting for a determinate period of time
                chaos::common::data::CDataWrapper *getResult();

                
                chaos::common::data::CDWUniquePtr detachResult();

                    //!return the request id
                uint32_t const & getRequestID();

                    //! return the code of the error
                int getError() const;

                    //! return the message of the error
                const std::string& getErrorDomain() const;

                    //! return the domain of the error
                const std::string& getErrorMessage() const;

                    //! identify a remote error o local remote
                bool isRemoteMeaning();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__MessageRequestFuture__) */
