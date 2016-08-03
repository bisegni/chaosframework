/*
 *	DatasetConsumer.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__A1E8B6A_044F_48C2_B53B_93FBD4647091_DatasetConsumer_h
#define __CHAOSFramework__A1E8B6A_044F_48C2_B53B_93FBD4647091_DatasetConsumer_h

#include <chaos/common/data/cache/AttributeValue.h>
#include <chaos/cu_toolkit//data_manager/trigger_system/AbstractEvent.h>
#include <chaos/cu_toolkit//data_manager/trigger_system/dataset_event/dataset_event_types.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                namespace dataset_event {
                    namespace cu_trigger = ::chaos::cu::data_manager::trigger_system;
                    //!define the abstract consumer for AbstractEventSetDSAttribute
                    class DatasetConsumer:
                    public ::chaos::cu::data_manager::trigger_system::AbstractConsumer {
                    public:
                        DatasetConsumer(const std::string& name,
                                        const std::string& description);
                        ~DatasetConsumer();
                        virtual cu_trigger::ConsumerResult consumeEvent(ETDatasetAttributeType event_type,
                                                                        const ::chaos::cu::data_manager::trigger_system::CDataVariantVector& event_values,
                                                                        chaos::common::data::cache::AttributeValue *attribute_cache) = 0;
                    };
                    
                    
                }
            }
        }
    }
}
#endif /* __CHAOSFramework__A1E8B6A_044F_48C2_B53B_93FBD4647091_DatasetConsumer_h */
