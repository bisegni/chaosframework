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

#include <chaos/common/trigger/AbstractConsumer.h>
#include <chaos/common/data/cache/AttributeValue.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/DatasetSubject.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/dataset_event_types.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                namespace dataset_event {
                    namespace chaos_trigger = chaos::common::trigger;
                    
                    //------create the consumer
                    CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(DatasetConsumerTest,
                                                            "Consumer test on dataset attribute",
                                                            ETDatasetAttributeType,
                                                            DatasetSubject)
                    
                    CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY_WFLAG("in_ds_name", "Input variable dataset name", chaos::DataType::TYPE_STRING, 1);
                    CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY_WFLAG("in_attr_name", "Input variable attribute name", chaos::DataType::TYPE_STRING, 1);
                    CHAOS_TRIGGER_CONSUMER_CLOSE_DESCRIPTION()
                }
            }
        }
    }
}
#endif /* __CHAOSFramework__A1E8B6A_044F_48C2_B53B_93FBD4647091_DatasetConsumer_h */
