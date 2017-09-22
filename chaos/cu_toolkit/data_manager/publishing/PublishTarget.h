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

#ifndef __CHAOSFramework__4C248F9_A814_4D6F_AA2A_279CC936E1D8_PublishTarget_h
#define __CHAOSFramework__4C248F9_A814_4D6F_AA2A_279CC936E1D8_PublishTarget_h

#include <chaos/common/chaos_constants.h>
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
                
                //identify a direct-io connection to an element of the target
                struct PublishTargetClientChannel {
                    chaos::common::direct_io::DirectIOClientConnection *connection;
                    chaos::common::direct_io::channel::DirectIODeviceClientChannel *device_client_channel;
                };
                
                struct PublishElementAttribute {
                    //!determinate the publishing mode for this dataset
                    chaos::DataServiceNodeDefinitionType::DSStorageType publishing_mode;
                    //!publish rate for dataset in the endpoint
                    uint64_t publish_rate;
                    
                    PublishElementAttribute();
                    
                    PublishElementAttribute(const PublishElementAttribute& src);
                    
                    //!overload assignement oeprator
                    PublishElementAttribute& operator=(const PublishElementAttribute& src);
                };
                
                //!represent a daataset that need to be publish into an endpoint
                /*!
                 correlate some publish information for the dataset for a specific endpoint
                 
                 */
                struct PublishableElement {
                    //! dataset to publish
                    const chaos::cu::data_manager::DatasetElement *dataset_ptr;
                    
                    PublishElementAttribute attribute;
                    
                    //!is the last timestamp when the dataset has been publishen into the endpoint
                    uint64_t last_publish_ts;
                    
                    PublishableElement();
                    
                    PublishableElement(const PublishableElement& _dataset_element);
                    
                    PublishableElement(const chaos::cu::data_manager::DatasetElement& _dataset_reference,
                                       const PublishElementAttribute& _attribute);
                };
                
                
                //!define a map between a string and a dataset publishable element
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, PublishableElement, PublishableElementMap);
                
                //!identify the target wehe need to be published the dataset
                /*!
                 Thi class identify an endpoint where one or more dataaccess need to be published. An endopoint
                 can be identified by a single url or a group of url for fileover-roundrobin high availability or
                 scale feature.
                 */
                class PublishTarget:
                public chaos::common::network::URLServiceFeederHandler,
                public chaos::common::utility::InizializableService,
                protected chaos::common::direct_io::DirectIOClientConnectionEventHandler {
                    //feed the rule to use the right directio channel for endpoint urls
                    ChaosSharedMutex                            mutext_feeder;
                    chaos::common::network::URLServiceFeeder    connection_feeder;
                    
                    //!map for correlate the name with the dataset publishable element
                    ChaosSharedMutex        mutext_map_pub;
                    PublishableElementMap   map_publishable_element;
                protected:
                    void disposeService(void *service_ptr);
                    
                    void *serviceForURL(const chaos::common::network::URL &url, uint32_t service_index);
                    
                    void handleEvent(chaos::common::direct_io::DirectIOClientConnection *client_connection,
                                     chaos::common::direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event);
                    
                    inline ChaosUniquePtr<chaos::common::data::CDataWrapper> getDataPack(const chaos::common::data::cache::AttributeCache &ac);
                    
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
                    void setAttributeOnDataset(const std::string& dataset_name,
                                               const PublishElementAttribute& publishable_attribute);
                    
                    //! add a new dataset to the end point
                    void addDataset(const chaos::cu::data_manager::DatasetElement &publishable_dataset,
                                    const PublishElementAttribute& publishable_attribute);
                    
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
