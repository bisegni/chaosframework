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
                    public ::chaos::common::trigger::AbstractEvent {
                    protected:
                        //collect consumer input data
                        chaos_trigger::CDataVariantVector consumer_input_value;
                        chaos_trigger::CDataVariantVector consumer_output_value;
                        
                    protected:
                        virtual chaos_trigger::ConsumerResult _executeConsumerOnTarget(DatasetSubject *attrbiute_subject_instance,
                                                                                       DatasetConsumer *consumer_instance) = 0;
                    public:
                        DatasetEvent(const std::string& _event_name,
                                     const std::string& _event_description,
                                     const ::chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeType _type);
                        const ::chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeType getType() const;
                        
                        chaos_trigger::ConsumerResult executeConsumerOnTarget(chaos_trigger::AbstractSubject *subject_instance,
                                                                              chaos_trigger::AbstractConsumer *consumer_instance);
                    };
                    
                    //! the event is forwarded befor an attribute value is changed
                    /*!
                     Pre change event collect into the CDataVariant vector for consumer these information:
                     [0]->new value
                     [1]->current_value
                     */
                    class EventDSAttributePreChange:
                    public DatasetEvent {
                    protected:
                        chaos_trigger::ConsumerResult _executeConsumerOnTarget(DatasetSubject *attribute_value,
                                                                               DatasetConsumer *consumer_instance);
                    public:
                        EventDSAttributePreChange(const chaos_data::CDataVariant& _new_value);
                        ~EventDSAttributePreChange();
                    };
                    
                    //! this event is forwarded befor an attribute value has been changed
                    class EventDSAttributePostChange:
                    public DatasetEvent {
                    protected:
                        chaos_trigger::ConsumerResult _executeConsumerOnTarget(DatasetSubject *attribute_value,
                                                                               DatasetConsumer *consumer_instance);
                    public:
                        EventDSAttributePostChange();
                        ~EventDSAttributePostChange();
                    };
                }
            }
        }
    }
}
#endif /* __CHAOSFramework__A590BCD_051A_4197_B57E_AE8E49060FB7_DatasetEvent_h */
