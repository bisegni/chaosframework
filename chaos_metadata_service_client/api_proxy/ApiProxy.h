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

#ifndef __CHAOSFramework__ApiProxy__
#define __CHAOSFramework__ApiProxy__

#include <chaos/common/chaos_types.h>
#include <chaos/common/message/MultiAddressMessageChannel.h>

#include <chaos_metadata_service_client/api_proxy/api_types.h>

#include <boost/atomic.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {

#define API_PROXY_CLASS(x) friend class INSTANCER_P2(x, ApiProxy, chaos::common::message::MultiAddressMessageChannel*, int32_t);

#define API_PROXY_CD_DECLARATION(x)\
explicit x(chaos::common::message::MultiAddressMessageChannel *_mn_message, int32_t timeout_in_milliseconds);\
//~x();

#define API_PROXY_CD_DEFINITION(x, group, name)\
x::x(chaos::common::message::MultiAddressMessageChannel *_mn_message, int32_t timeout_in_milliseconds):ApiProxy(group, name, _mn_message, timeout_in_milliseconds){}\
//x::~x(){}

#define API_PROXY_CD_DEFINITION_WITH_INIT(x, group, name, init)\
x::x(chaos::common::message::MultiAddressMessageChannel *_mn_message, int32_t timeout_in_milliseconds):ApiProxy(group, name, _mn_message, timeout_in_milliseconds){init}\
//x::~x(){}
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
                    //! execute an api call

            public:
                /*!
                 preform an call of an api and return the opportune future
                  To be used directly i.e rest call with json params

                 that permit to inspect the result or error
                 */
                ApiProxyResult callApi(const chaos::common::data::CDWUniquePtr& api_message = chaos::common::data::CDWUniquePtr());

                //! default destructor
                ~ApiProxy();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__ApiProxy__) */
