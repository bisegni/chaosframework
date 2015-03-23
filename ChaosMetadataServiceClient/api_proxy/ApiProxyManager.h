/*
 *	ApiProxyManager.h
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
#ifndef __CHAOSFramework__ApiProxyManager__
#define __CHAOSFramework__ApiProxyManager__

#include <ChaosMetadataServiceClient/metadata_service_client_types.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <string>
#include <vector>

namespace chaos {
    namespace metadata_service_client {
            //! forward declaration
        class ChaosMetadataServiceClient;

        namespace api_proxy {
                //type
            typedef std::vector< boost::shared_ptr<ApiProxy> >           ApiProxyList;
            typedef std::vector< boost::shared_ptr<ApiProxy> >::iterator ApiProxyListIterator;

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
                void getProxyApi() {
                        //! there was a type for every tempalte expantion
                    static P* instance = NULL;
                    if(instance) {
                        //allcoate the instsancer for the AbstractApi depending by the template
                        std::auto_ptr<INSTANCER_P1(P, ApiProxy, chaos::common::message::MultiAddressMessageChannel*)> i(ALLOCATE_INSTANCER_P1(P, ApiProxy, chaos::common::message::MultiAddressMessageChannel*));

                        //get api instance
                        instance = (P*)i->getInstance(mn_message_channel);

                            //add new instance to the list of all instances
                        api_instance.push_back(boost::shared_ptr<ApiProxy>(instance));
                    }
                    return instance;
                };
                    //!inherited by InizializableService
                void init(void *init_data) throw (chaos::CException);
                    //!inherited by InizializableService
                void deinit() throw (chaos::CException);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__ApiProxyManager__) */
