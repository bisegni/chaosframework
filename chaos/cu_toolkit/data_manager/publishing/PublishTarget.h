/*
 *	PublishTarget.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 12/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__4C248F9_A814_4D6F_AA2A_279CC936E1D8_PublishTarget_h
#define __CHAOSFramework__4C248F9_A814_4D6F_AA2A_279CC936E1D8_PublishTarget_h

#include <chaos/common/network/URLServiceFeeder.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/data/cache/AttributeCache.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>

#include <chaos/cu_toolkit/data_manager/publishing/publishing_types.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace publishing {
                
                //!identify the target wehe need to be published the dataset
                /*!
                 Thi class identify an endpoint where one or more dataaccess need to be published. An endopoint
                 can be identified by a single url or a group of url for fileover-roundrobin high availability or
                 scale feature.
                 */
                class PublishTarget:
                public chaos::common::network::URLServiceFeederHandler,
                public chaos::common::utility::InizializableService,
                protected chaos_direct_io::DirectIOClientConnectionEventHandler {
                    //feed the rule to use the right directio channel for endpoint urls
                    ChaosSharedMutex mutext_feeder;
                    chaos::common::network::URLServiceFeeder connection_feeder;

                    //!map for correlate the name with the dataset publishable element
                    ChaosSharedMutex mutext_map_pub;
                    PublishableElementMap map_publishable_element;

                protected:
                    void disposeService(void *service_ptr);
                    
                    void *serviceForURL(const chaos::common::network::URL &url, uint32_t service_index);
                    
                    void handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
                                     chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event);
                    
                    inline std::auto_ptr<chaos::common::data::CDataWrapper> getDataPack(const chaos::common::data::cache::AttributeCache &ac);
                    
                    //!publis the dataset into the target
                    bool publish(const PublishableElement& publishable_dataset);
                public:
                    PublishTarget(const std::string &target_name);
                    ~PublishTarget();
                    
                    //!remove all registered url erasing the connections
                    void clear();
                    
                    //! add a new endpoint to the target
                    bool addServer(const std::string &server_url_new);
                    
                    //! remove and endpoint identified by url form the target
                    bool removeServer(const std::string &server_url_erase);
                    
                    //!change the publishing mode for this target
                    void setPublishingModeOnDataset(const std::string& dataset_name,
                                                    common::direct_io::channel::DirectIODeviceClientChannelPutMode new_publishing_mode);
                    void setPublishingRateOnDataset(const std::string& dataset_name,
                                                    const uint64_t new_rate);
                    //! add a new dataset to the end point
                    void addDataset(const chaos::cu::data_manager::DatasetElement &publishable_dataset,
                                    const common::direct_io::channel::DirectIODeviceClientChannelPutMode  new_publishing_mode,
                                    const uint64_t push_rate);
                    
                    //! remove a dataset by his name
                    void removeDataset(const std::string& dataset_name);
                  
                    //!publish all registered dataset into the endpoint
                    void publish();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__4C248F9_A814_4D6F_AA2A_279CC936E1D8_PublishTarget_h */
