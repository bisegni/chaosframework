/*
 *	ApiProxyManager.h
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
#ifndef __CHAOSFramework__ApiProxyManager__
#define __CHAOSFramework__ApiProxyManager__

#include <ChaosMetadataServiceClient/metadata_service_client_types.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <string>
#include <vector>

#define GET_CHAOS_API_PTR(api_name)\
    chaos::metadata_service_client::ChaosMetadataServiceClient::getInstance()->getApiProxy<api_name>()

namespace chaos {
    namespace metadata_service_client {
            //! forward declaration
        class ChaosMetadataServiceClient;

        namespace api_proxy {
                //type
            typedef std::vector< ApiProxy* >           ApiProxyList;
            typedef std::vector< ApiProxy* >::iterator ApiProxyListIterator;

                //! Manager for the creation of the appi proxy and thei managment
            /*!
             For every MDS remote api there is a client proxy that hide the messaging
             complessity to the client
             */
            class ApiProxyManager:
            public chaos::common::utility::InizializableService {
                friend class chaos::common::utility::InizializableServiceContainer<ApiProxyManager>;
                friend class chaos::metadata_service_client::ChaosMetadataServiceClient;

                    //! Applciation settings
                ClientSetting *setting;
                    //! list of the api instance
                ApiProxyList api_instance;

                    //! network broker service
                chaos::common::network::NetworkBroker *network_broker;

                    //! referecne to the multinode message channel
                chaos::common::message::MultiAddressMessageChannel *mn_message_channel;


                    //! default private constructor called only by ChaosMetadataServiceClient class
                ApiProxyManager(chaos::common::network::NetworkBroker *_network_broker,
                                ClientSetting *_setting);

                    //! default destructor called only by ChaosMetadataServiceClient class
                ~ApiProxyManager();
            public:
                template<typename P>
                P* getApiProxy(int32_t timeout_in_milliseconds = 1000) {
                        //! there was a type for every template expantion
                    static P* instance = NULL;
                    if(instance == NULL) {
                        //allcoate the instsancer for the AbstractApi depending by the template
                        std::auto_ptr<INSTANCER_P2(P, ApiProxy, chaos::common::message::MultiAddressMessageChannel*, int32_t)> i(ALLOCATE_INSTANCER_P2(P, ApiProxy, chaos::common::message::MultiAddressMessageChannel*, int32_t));

                        //get api instance
                        instance = (P*)i->getInstance(mn_message_channel, timeout_in_milliseconds);

                            //add new instance to the list of all instances
                        api_instance.push_back(instance);
                    }
                    return instance;
                };
                    //!inherited by InizializableService
                void init(void *init_data) throw (chaos::CException);
                    //!inherited by InizializableService
                void deinit() throw (chaos::CException);
                    //! add a new api server endpoint
                void addServerAddress(const std::string& server_address);
                    //! remove all api endpoint server
                void clearServer();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__ApiProxyManager__) */
