/*
 *	publishing_type.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 18/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_B829D1BF_E7DA_40E8_96F6_E91815C2ADAB_publishing_type_h
#define __CHAOSFramework_B829D1BF_E7DA_40E8_96F6_E91815C2ADAB_publishing_type_h

#include <chaos/cu_toolkit/data_manager/data_manager_types.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace publishing {
                
                
                //identify a direct-io connection to an element of the target
                struct PublishTargetClientChannel {
                    chaos::common::direct_io::DirectIOClientConnection *connection;
                    chaos::common::direct_io::channel::DirectIODeviceClientChannel *device_client_channel;
                };
                
                //!represent a daataset that need to be publish into an endpoint
                /*!
                 correlate some publish information for the dataset for a specific endpoint
                 
                 */
                struct PublishableElement {
                    //! dataset to publish
                    const chaos::cu::data_manager::DatasetElement *dataset_ptr;
                    //!determinate the publishing mode for this dataset
                    common::direct_io::channel::DirectIODeviceClientChannelPutMode publishing_mode;
                    //!publish rate for dataset in the endpoint
                    uint64_t publish_rate;
                    //!is the last timestamp when the dataset has been publishen into the endpoint
                    uint64_t last_publish_ts;
                    
                    PublishableElement();
                    
                    PublishableElement(const PublishableElement& _dataset_element);
                    
                    PublishableElement(const chaos::cu::data_manager::DatasetElement& _dataset_reference,
                                       const common::direct_io::channel::DirectIODeviceClientChannelPutMode _publishing_mode,
                                       const uint64_t publish_rate);
                };
                
                //!define a map between a string and a dataset publishable element
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, PublishableElement, PublishableElementMap);
            }
        }
    }
}

#endif /* __CHAOSFramework_B829D1BF_E7DA_40E8_96F6_E91815C2ADAB_publishing_type_h */
