/*
 *	ApiProxy.h
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

#ifndef __CHAOSFramework__ApiProxy__
#define __CHAOSFramework__ApiProxy__

#include <chaos/common/chaos_types.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>

#include <ChaosMetadataServiceClient/api_proxy/api_types.h>

#include <boost/atomic.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {

#define API_PROXY_CLASS(x) friend class INSTANCER_P2(x, ApiProxy, chaos::common::message::MultiAddressMessageChannel*, int32_t);

#define API_PROXY_CD_DECLARATION(x)\
explicit x(chaos::common::message::MultiAddressMessageChannel *_mn_message, int32_t timeout_in_milliseconds);\
~x();

#define API_PROXY_CD_DEFINITION(x, group, name)\
x::x(chaos::common::message::MultiAddressMessageChannel *_mn_message, int32_t timeout_in_milliseconds):ApiProxy(group, name, _mn_message, timeout_in_milliseconds){}\
x::~x(){}

#define API_PROXY_CD_DEFINITION_WITH_INIT(x, group, name, init)\
x::x(chaos::common::message::MultiAddressMessageChannel *_mn_message, int32_t timeout_in_milliseconds):ApiProxy(group, name, _mn_message, timeout_in_milliseconds){init}\
x::~x(){}
            //! define the result of an api like shared pointer of @chaos::common::message::MessageRequestFuture
            typedef ChaosUniquePtr<chaos::common::message::MultiAddressMessageRequestFuture> ApiProxyResult;
            
            class ApiResultHelper {
            protected:
                chaos::common::data::CDataWrapper *api_result;
            public:
                ApiResultHelper(chaos::common::data::CDataWrapper *_api_result);
                virtual ~ApiResultHelper();
            };
            
            class ApiProxyManager;
                //! base class for all proxy api
            class ApiProxy {
                friend class ApiProxyManager;
                    //!keep track on how many access are done to the proxy
                boost::atomic<int> access_counter;
                    //!is the timeout for the future
                int32_t timeout_in_milliseconds;
                    //!channel for mds communication
                chaos::common::message::MultiAddressMessageChannel *mn_message;
            protected:
                    //! point to api group name
                std::string     group_name;
                    //! point to the api name
                std::string     api_name;

                    //! default constructor
                explicit ApiProxy(const std::string& _group_name,
                                  const std::string& _api_name,
                                  chaos::common::message::MultiAddressMessageChannel *_mn_message,
                                  int32_t _timeout_in_milliseconds = 1000);
                    //! default destructor
                ~ApiProxy();
                    //! execute an api call
                /*!
                 preform an call of an api and return the opportune future
                 that permit to inspect the result or error
                 */
                ApiProxyResult callApi(chaos::common::data::CDataWrapper *api_message = NULL);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__ApiProxy__) */
