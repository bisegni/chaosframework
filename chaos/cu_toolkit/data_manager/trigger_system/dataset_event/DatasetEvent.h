/*
 *	DatasetEvent.h
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

#ifndef __CHAOSFramework__A590BCD_051A_4197_B57E_AE8E49060FB7_DatasetEvent_h
#define __CHAOSFramework__A590BCD_051A_4197_B57E_AE8E49060FB7_DatasetEvent_h

#include <chaos/common/data/cache/AttributeValue.h>
#include <chaos/common/trigger/AbstractEvent.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/DatasetSubject.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/dataset_event_types.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                namespace dataset_event {
                    namespace chaos_data = chaos::common::data;
                    namespace chaos_trigger = chaos::common::trigger;
                    
                    //!forward decalration
                    class DatasetConsumer;
                    
                    //!define the event for attribute change
                    class DatasetEvent:
                    public ::chaos::common::trigger::AbstractEvent<chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeType, DatasetSubject> {
                    protected:
                        
                    protected:
                        
                    public:
                        DatasetEvent(const std::string& _event_name,
                                     const std::string& _event_description,
                                     const ::chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeType _type);
                        const ::chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeType getType() const;
                    };
                    
                    //! the event is forwarded befor an attribute value is changed
                    CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(EventDSAttributePreChange,
                                                            DatasetEvent)
                    //CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY("offset", "amount of int's added to 1", chaos::DataType::TYPE_INT32);
                    CHAOS_TRIGGER_CONSUMER_ADD_DEFINITION_1P(EventDSAttributePreChange,
                                                             DatasetEvent,
                                                             "Signal for pre change attribute value",
                                                             ::chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeTypePreChange)
                    chaos_trigger::ConsumerResult executeConsumerOnTarget(DatasetSubject *attribute_value,
                                                                          chaos_trigger::AbstractConsumer<chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeType, DatasetSubject> *consumer_instance);
                    CHAOS_TRIGGER_CONSUMER_CLOSE_DEFINITION()
                    
                    
                    //! this event is forwarded befor an attribute value has been changed
                    CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(EventDSAttributePostChange,
                                                            DatasetEvent)
                    //CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY("offset", "amount of int's added to 1", chaos::DataType::TYPE_INT32);
                    CHAOS_TRIGGER_CONSUMER_ADD_DEFINITION_1P(EventDSAttributePostChange,
                                                             DatasetEvent,
                                                             "Signal for pre change attribute value",
                                                             ::chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeTypePostChange)
                    chaos_trigger::ConsumerResult executeConsumerOnTarget(DatasetSubject *attribute_value,
                                                                          chaos_trigger::AbstractConsumer<chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeType, DatasetSubject> *consumer_instance);
                    CHAOS_TRIGGER_CONSUMER_CLOSE_DEFINITION()
                }
            }
        }
    }
}
#endif /* __CHAOSFramework__A590BCD_051A_4197_B57E_AE8E49060FB7_DatasetEvent_h */
