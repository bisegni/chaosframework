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

#ifndef CHAOS_MESSAGEREQUESTDOMAIN_H
#define CHAOS_MESSAGEREQUESTDOMAIN_H

#include <chaos/common/chaos_types.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/Atomic.h>
#include <chaos/common/utility/SafeAsyncCall.h>

#include <chaos/common/message/MessageRequestFuture.h>

#include <boost/atomic.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
//#define BOOST_THREAD_PROVIDES_FUTURE
#include <boost/thread/future.hpp>

#include <map>
#include <string>

namespace chaos {
    namespace common {
        namespace message {
            class MessageChannel;
            class MessageRequestFuture;
            class MessageRequestDomain;
            
            typedef ChaosSharedPtr<chaos::common::message::MessageRequestDomain> MessageRequestDomainSHRDPtr;
            
            typedef ChaosSharedPtr<common::data::CDataWrapper> FuturePromiseData;
            typedef ChaosPromise<FuturePromiseData> MessageFuturePromise;
            typedef ChaosFunction<void(const FuturePromiseData&)> PromisesHandlerFunction;
            
            
            typedef chaos::common::utility::SafeAsyncCall<PromisesHandlerFunction> PromisesHandler;
            typedef ChaosSharedPtr< PromisesHandler > PromisesHandlerSharedPtr;
            typedef ChaosWeakPtr< PromisesHandler > PromisesHandlerWeakPtr;
            
            //! manage the RC domain for the request of a message channel
            class MessageRequestDomain:
            public DeclareAction {
                std::string domain_id;
                MessageRequestDomainFutureHelper future_helper;
            protected:
                /*!
                 Called when a response to a request is received, it will manage the search of
                 the apprioriate promis and answer to it
                 */
                virtual chaos::common::data::CDataWrapper *response(chaos::common::data::CDataWrapper *reposnse_data,
                                                                    bool &detach);
            public:
                MessageRequestDomain();
                ~MessageRequestDomain();
                
                const std::string& getDomainID();
                
                ChaosUniquePtr<MessageRequestFuture> getNewRequestMessageFuture(chaos::common::data::CDataWrapper& new_request_datapack,
                                                                               uint32_t& new_request_id,
                                                                                MessageRequestDomainFutureHelper::PromisesHandlerWeakPtr promises_handler_weak);
            };
        }
    }
}
#endif //CHAOS_MESSAGEREQUESTDOMAIN_H
