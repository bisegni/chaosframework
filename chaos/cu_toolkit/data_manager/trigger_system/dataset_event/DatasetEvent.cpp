/*
 *	DatasetEvent.cpp
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

#include <chaos/common/data/cache/AttributeValue.h>

#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/DatasetEvent.h>
#include <chaos/cu_toolkit//data_manager/trigger_system/dataset_event/DatasetConsumer.h>

using namespace chaos::common::data;
using namespace chaos::common::data::cache;
using namespace chaos::cu::data_manager::trigger_system;
using namespace chaos::cu::data_manager::trigger_system::dataset_event;


#pragma mark DatasetEvent
DatasetEvent::DatasetEvent(const std::string& _event_name,
                                                   const ETDatasetAttributeType _type):
AbstractEvent("EventDatasetAttribute",
              _event_name,
              _type){}

const ETDatasetAttributeType DatasetEvent::getType() const {
    return static_cast<ETDatasetAttributeType>(AbstractEvent::getEventCode());
}

ConsumerResult DatasetEvent::executeConsumerOnTarget(AbstractSubject *subject_instance,
                                                                 AbstractConsumer *consumer_instance) {
    return _executeConsumerOnTarget(static_cast<DatasetSubject*>(subject_instance),
                                    static_cast<DatasetConsumer*>(consumer_instance));
}

#pragma mark EventDSAttributePreChange
EventDSAttributePreChange::EventDSAttributePreChange(const CDataVariant& _new_value):
DatasetEvent("EventDSAttributePreChange",
                         ETDatasetAttributeTypePreChange) {consumer_input_value.push_back(_new_value);}

EventDSAttributePreChange::~EventDSAttributePreChange() {}

ConsumerResult EventDSAttributePreChange::_executeConsumerOnTarget(DatasetSubject *attribute_subject_instance,
                                                                   DatasetConsumer *consumer_instance) {
    ConsumerResult err = ConsumerResultOK;
    //add to the vector the CDataVariant from attribute value
    consumer_input_value.push_back(attribute_subject_instance->getDatasetCache()->getAsVariant());
    err = consumer_instance->consumeEvent(getType(),
                                          consumer_input_value,
                                          consumer_output_value);
    return err;
}

#pragma mark EventDSAttributePreChange
EventDSAttributePostChange::EventDSAttributePostChange():
DatasetEvent("EventDSAttributePostChange",
                         ETDatasetAttributeTypePostChange){}

EventDSAttributePostChange::~EventDSAttributePostChange() {}

ConsumerResult EventDSAttributePostChange::_executeConsumerOnTarget(DatasetSubject *attribute_subject_instance,
                                                                    DatasetConsumer *consumer_instance) {
    consumer_input_value.push_back(attribute_subject_instance->getDatasetCache()->getAsVariant());
    ConsumerResult err = consumer_instance->consumeEvent(getType(),
                                                         consumer_input_value,
                                                         consumer_output_value);
    return err;
}
