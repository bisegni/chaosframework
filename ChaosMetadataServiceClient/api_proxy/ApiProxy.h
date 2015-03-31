/*
 *	ApiProxy.h
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

#ifndef __CHAOSFramework__ApiProxy__
#define __CHAOSFramework__ApiProxy__

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>

#include <boost/atomic.hpp>

#define API_PROXY_CLASS(x) friend class INSTANCER_P1(x, ApiProxy, chaos::common::message::MultiAddressMessageChannel*);

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            
            //! define the result of an api like shared pointer of @chaos::common::message::MessageRequestFuture
            typedef auto_ptr<chaos::common::message::MessageRequestFuture> ApiProxyResult;
            
            class ApiProxyManager;
                //! base class for all proxy api
            class ApiProxy:
            public chaos::common::utility::NamedService {
                friend class ApiProxyManager;
                    //!keep track on how many access are done to the proxy
                boost::atomic<int> access_counter;
                    //!channel for mds communication
                chaos::common::message::MultiAddressMessageChannel *mn_message;
            protected:
                    //! default constructor
                ApiProxy(const std::string& api_name,
                         chaos::common::message::MultiAddressMessageChannel *_mn_message);
                    //! default destructor
                ~ApiProxy();
            protected:
                    //! execute an api call
                /*!
                 preform an call of an api and return the opportune future
                 that permit to inspect the result or error
                 */
                ApiProxyResult callApi(const std::string& api_group,
                                       const std::string& api_name,
                                       chaos::common::data::CDataWrapper *api_message);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__ApiProxy__) */
