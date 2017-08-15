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
#ifndef __CHAOSFramework__ApiProxyManager__
#define __CHAOSFramework__ApiProxyManager__

#include <ChaosMetadataServiceClient/metadata_service_client_types.h>
#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <string>
#include <vector>
#include <iostream>
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
                
                //! referecne to the multinode message channel
                chaos::common::message::MultiAddressMessageChannel *mn_message_channel;
                
                
                //! default private constructor called only by ChaosMetadataServiceClient class
                ApiProxyManager();
                
                //! default destructor called only by ChaosMetadataServiceClient class
                ~ApiProxyManager();
            public:
                template<typename P>
                ChaosUniquePtr<P> getApiProxy(int32_t timeout_in_milliseconds = RpcConfigurationKey::GlobalRPCTimeoutinMSec) {
                    static ChaosUniquePtr<INSTANCER_P2(P, ApiProxy, chaos::common::message::MultiAddressMessageChannel*, int32_t)> i(ALLOCATE_INSTANCER_P2(P, ApiProxy, chaos::common::message::MultiAddressMessageChannel*, int32_t));
                    //! there was a type for every template expantion
                    return ChaosUniquePtr<P>((P*)i->getInstance(mn_message_channel, timeout_in_milliseconds));
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
