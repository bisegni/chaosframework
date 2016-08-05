/*
 *	TestEvent.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/08/16 INFN, National Institute of Nuclear Physics
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

#include "TestEvent.h"

#include <chaos/common/property/PropertyGroup.h>

using namespace chaos::common::data;
using namespace chaos::test::trigger_system;

TriggeredData::TriggeredData(int _new_value):
data_value(_new_value){}
//---------------------------------

Subject::Subject(const TriggeredDataShrdPtr& _subject_data):
AbstractSubject("TriggeredData"),
subject_data(_subject_data){}
//---------------------------------

SubjectEvent::SubjectEvent(const std::string& _event_name,
                           const std::string& _event_description,
                           const TriggerDataEventType _type):
AbstractEvent(_event_name,
              _event_description,
              _type){}

ConsumerResult SubjectEvent::executeConsumerOnTarget(AbstractSubject *subject_instance,
                                                     AbstractConsumer *consumer_instance) {
    return _executeConsumerOnTarget(static_cast<Subject*>(subject_instance),
                                    static_cast<SubjectConsumer*>(consumer_instance));
}
//---------------------------------
SubjectConsumer::SubjectConsumer(const std::string& name,
                                 const std::string& description):
AbstractConsumer(name,
                 description){}
SubjectConsumer::~SubjectConsumer(){}
//---------------------------------
SubjetEventOne::SubjetEventOne():
SubjectEvent("SubjetEventOne",
             "Occures on case two",
             TriggerDataEventTypeOne){}
SubjetEventOne::SubjetEventOne(const int increment_grow_factor):
SubjectEvent("SubjetEventOne",
             "Occures on case two",
             TriggerDataEventTypeOne){
    consumer_input_value.push_back(CDataVariant(increment_grow_factor));
}
ConsumerResult SubjetEventOne::_executeConsumerOnTarget(Subject *subject_instance,
                                                        SubjectConsumer *consumer_instance) {
    return consumer_instance->consumeEvent(static_cast<TriggerDataEventType>(getEventCode()),
                                           *subject_instance->subject_data,
                                           consumer_input_value);
}
//---------------------------------

SubjetEventTwo::SubjetEventTwo():
SubjectEvent("SubjetEventTwo",
             "Occures on case two",
             TriggerDataEventTypeTwo){}

SubjetEventTwo::SubjetEventTwo(const int decrement_grow_factor):
SubjectEvent("SubjetEventTwo",
             "Occures on case two",
             TriggerDataEventTypeTwo){
    consumer_input_value.push_back(CDataVariant(decrement_grow_factor));
}

ConsumerResult SubjetEventTwo::_executeConsumerOnTarget(Subject *subject_instance,
                                                        SubjectConsumer *consumer_instance) {
    return consumer_instance->consumeEvent(static_cast<TriggerDataEventType>(getEventCode()),
                                           *subject_instance->subject_data,
                                           consumer_input_value);
}
//---------------------------------
ConsumerResult SubjectConsumerIncrement::consumeEvent(TriggerDataEventType event_type,
                                                      TriggeredData& trigger_data,
                                                      const CDataVariantVector& event_values) {
    int32_t incremnet_size = 0;
    if(event_values.size() == 1) {
        //get increment size
        incremnet_size = event_values[0].asInt32();
    }
    trigger_data.data_value += (1+incremnet_size);
    return ConsumerResultOK;
}

//---------------------------------
ConsumerResult SubjectConsumerDecrement::consumeEvent(TriggerDataEventType event_type,
                                                      TriggeredData& trigger_data,
                                                      const CDataVariantVector& event_values) {
    int32_t incremnet_size = 0;
    if(event_values.size() == 1) {
        //get increment size
        incremnet_size = event_values[0].asInt32();
    }
    trigger_data.data_value -= (1+incremnet_size);
    return ConsumerResultOK;
}

//---------------------------------
TestEvent::TestEvent(){
    
}

TestEvent::~TestEvent(){
    
}

bool TestEvent::test(){
    //instantiate the trigger environment
    SubjectTriggerEnviroment trigger_environment;
    
    //create the subjects
    SubjectTriggerEnviroment::SubjectInstanceShrdPtr subject_one(boost::make_shared<Subject>(boost::make_shared<TriggeredData>(0)));
    SubjectTriggerEnviroment::SubjectInstanceShrdPtr subject_two(boost::make_shared<Subject>(boost::make_shared<TriggeredData>(0)));
    
    trigger_environment.registerSubject(subject_one);
    trigger_environment.registerSubject(subject_two);
    
    //attach conusmer to subject
    assert(trigger_environment.addConsumerOnSubjectForEvent(TriggerDataEventTypeOne,
                                                            subject_one,
                                                            "SubjectConsumerIncrement"));
    
    assert(trigger_environment.addConsumerOnSubjectForEvent(TriggerDataEventTypeTwo,
                                                            subject_two,
                                                            "SubjectConsumerDecrement"));
    
    assert(subject_one->subject_data->data_value == 0);
    assert(subject_two->subject_data->data_value == 0);
    
    trigger_environment.fireEventOnSubject(SubjectTriggerEnviroment::EventInstanceShrdPtr(new SubjetEventOne()), subject_one);
    assert(subject_one->subject_data->data_value == 1);
    assert(subject_two->subject_data->data_value == 0);
    
    trigger_environment.fireEventOnSubject(SubjectTriggerEnviroment::EventInstanceShrdPtr(new SubjetEventOne(10)), subject_one);
    assert(subject_one->subject_data->data_value == 12);
    assert(subject_two->subject_data->data_value == 0);
    
    trigger_environment.fireEventOnSubject(SubjectTriggerEnviroment::EventInstanceShrdPtr(new SubjetEventTwo()), subject_two);
    assert(subject_one->subject_data->data_value == 12);
    assert(subject_two->subject_data->data_value == -1);
    
    trigger_environment.fireEventOnSubject(SubjectTriggerEnviroment::EventInstanceShrdPtr(new SubjetEventTwo(10)), subject_two);
    assert(subject_one->subject_data->data_value == 12);
    assert(subject_two->subject_data->data_value == -12);
    
    //add xross consumer and fire event to reset the counter in the subject
    assert(trigger_environment.addConsumerOnSubjectForEvent(TriggerDataEventTypeTwo,
                                                            subject_one,
                                                            "SubjectConsumerDecrement"));
    
    assert(trigger_environment.addConsumerOnSubjectForEvent(TriggerDataEventTypeOne,
                                                            subject_two,
                                                            "SubjectConsumerIncrement"));
    
    trigger_environment.fireEventOnSubject(SubjectTriggerEnviroment::EventInstanceShrdPtr(new SubjetEventTwo()), subject_one);
    trigger_environment.fireEventOnSubject(SubjectTriggerEnviroment::EventInstanceShrdPtr(new SubjetEventTwo(10)), subject_one);
    trigger_environment.fireEventOnSubject(SubjectTriggerEnviroment::EventInstanceShrdPtr(new SubjetEventOne()), subject_two);
    trigger_environment.fireEventOnSubject(SubjectTriggerEnviroment::EventInstanceShrdPtr(new SubjetEventOne(10)), subject_two);
    assert(subject_one->subject_data->data_value == 0);
    assert(subject_two->subject_data->data_value == 0);
    
    //add consumer decrement to event one to subject one in this case counter need to remain to 0
    assert(trigger_environment.addConsumerOnSubjectForEvent(TriggerDataEventTypeOne,
                                                            subject_one,
                                                            "SubjectConsumerDecrement"));
    trigger_environment.fireEventOnSubject(SubjectTriggerEnviroment::EventInstanceShrdPtr(new SubjetEventOne()), subject_one);
    assert(subject_one->subject_data->data_value == 0);
    
    trigger_environment.fireEventOnSubject(SubjectTriggerEnviroment::EventInstanceShrdPtr(new SubjetEventOne(10)), subject_one);
    assert(subject_one->subject_data->data_value == 0);
    
    
    //test property group
    //common::property::PorpertyGroup p_group("test");
    //p_group.addProperty("prop_a", "good property", chaos::DataType::TYPE_INT32);
    //p_group.addProperty("prop_b", "good property", chaos::DataType::TYPE_INT64);
    //p_group.addProperty("prop_c", "good property", chaos::DataType::TYPE_DOUBLE);
    SubjectConsumerIncrementPropertyDescription cons_desc;
    common::property::PorpertyGroupSDWrapper group_ref_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(common::property::PorpertyGroup, cons_desc));
    std::cout << group_ref_wrapper.serialize()->getJSONString() << std::endl;
    return true;
}
