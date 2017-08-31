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

#include "TestTrigger.h"

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
ConsumerResult SubjectConsumerIncrement::consumeEvent(TriggerDataEventType event_type,
                                                      Subject& trigger_data) {
    int32_t incremnet_size = 0;
    const CDataVariant& inc_value = getPropertyValue("offset");
    if(inc_value.isValid()) {
        //get increment size
        incremnet_size = inc_value.asInt32();
    }
    trigger_data.subject_data->data_value += (1+incremnet_size);
    return ConsumerResultOK;
}

//---------------------------------
ConsumerResult SubjectConsumerDecrement::consumeEvent(TriggerDataEventType event_type,
                                                      Subject& trigger_data) {
    int32_t incremnet_size = 0;
    const CDataVariant& dec_value = getPropertyValue("offset");
    if(dec_value.isValid()) {
        
        //get increment size
        incremnet_size = dec_value.asInt32();
    }
    trigger_data.subject_data->data_value -= (1+incremnet_size);
    return ConsumerResultOK;
}

//---------------------------------
TestTrigger::TestTrigger(){
    
}

TestTrigger::~TestTrigger(){
    
}

bool TestTrigger::test(){
    //instantiate the trigger environment
    SubjectTriggerEnviroment trigger_environment;
    
    //create the subjects
    SubjectTriggerEnviroment::SubjectInstanceShrdPtr subject_one(ChaosMakeSharedPtr<Subject>(ChaosMakeSharedPtr<TriggeredData>(0)));
    SubjectTriggerEnviroment::SubjectInstanceShrdPtr subject_two(ChaosMakeSharedPtr<Subject>(ChaosMakeSharedPtr<TriggeredData>(0)));
    
    trigger_environment.registerSubject(subject_one);
    trigger_environment.registerSubject(subject_two);
    
    //attach conusmer to subject
    assert(trigger_environment.addConsumerOnSubjectForEvent(kTriggerDataEventTypeOne,
                                                            subject_one,
                                                            "SubjectConsumerIncrement"));
    
    assert(trigger_environment.addConsumerOnSubjectForEvent(kTriggerDataEventTypeTwo,
                                                            subject_two,
                                                            "SubjectConsumerDecrement"));
    
    SubjectTriggerEnviroment::EventInstanceShrdPtr event_one = trigger_environment.getEventInstance(kTriggerDataEventTypeOne);
    assert(event_one.get());
    SubjectTriggerEnviroment::EventInstanceShrdPtr event_two = trigger_environment.getEventInstance(kTriggerDataEventTypeTwo);
    assert(event_two.get());
    
    assert(subject_one->subject_data->data_value == 0);
    assert(subject_two->subject_data->data_value == 0);
    
    trigger_environment.fireEventOnSubject(event_one, subject_one);
    assert(subject_one->subject_data->data_value == 1);
    assert(subject_two->subject_data->data_value == 0);
    
    event_one->getProperties()("offset", CDataVariant(10));
    trigger_environment.fireEventOnSubject(event_one, subject_one);
    assert(subject_one->subject_data->data_value == 12);
    assert(subject_two->subject_data->data_value == 0);
    
    trigger_environment.fireEventOnSubject(event_two, subject_two);
    assert(subject_one->subject_data->data_value == 12);
    assert(subject_two->subject_data->data_value == -1);
    
    event_two->getProperties()("offset", CDataVariant(10));
    trigger_environment.fireEventOnSubject(event_two, subject_two);
    assert(subject_one->subject_data->data_value == 12);
    assert(subject_two->subject_data->data_value == -12);
    
    //add xross consumer and fire event to reset the counter in the subject
    assert(trigger_environment.addConsumerOnSubjectForEvent(kTriggerDataEventTypeTwo,
                                                            subject_one,
                                                            "SubjectConsumerDecrement"));
    
    assert(trigger_environment.addConsumerOnSubjectForEvent(kTriggerDataEventTypeOne,
                                                            subject_two,
                                                            "SubjectConsumerIncrement"));
    event_two->getProperties()();//reset the values;
    trigger_environment.fireEventOnSubject(event_two, subject_one);
    
    event_two->getProperties()("offset", CDataVariant(10));
    trigger_environment.fireEventOnSubject(event_two, subject_one);
    
    event_one->getProperties()();//reset the value
    trigger_environment.fireEventOnSubject(event_one, subject_two);
    event_one->getProperties()("offset", CDataVariant(10));
    trigger_environment.fireEventOnSubject(event_one, subject_two);
    assert(subject_one->subject_data->data_value == 0);
    assert(subject_two->subject_data->data_value == 0);
    
    //add consumer decrement to event one to subject one in this case counter need to remain to 0
    assert(trigger_environment.addConsumerOnSubjectForEvent(kTriggerDataEventTypeOne,
                                                            subject_one,
                                                            "SubjectConsumerDecrement"));
    event_one->getProperties()();
    trigger_environment.fireEventOnSubject(event_one, subject_one);
    assert(subject_one->subject_data->data_value == 0);
    
    event_one->getProperties()("offset", CDataVariant(10));
    trigger_environment.fireEventOnSubject(event_one, subject_one);
    assert(subject_one->subject_data->data_value == 0);
    
    int idx = 0;
    SubjectTriggerEnviroment::VectorConsumerInstance consumer_list;
    trigger_environment.getConsumerListBy(kTriggerDataEventTypeOne, subject_one, consumer_list);
    for(SubjectTriggerEnviroment::VectorConsumerInstanceIterator it = consumer_list.begin(),
        end = consumer_list.end();
        it != end;
        it++, idx++) {
        switch (idx) {
            case 0:
                assert((*it)->getConsumerName().compare("SubjectConsumerIncrement") == 0);
                break;
            case 1:
                assert((*it)->getConsumerName().compare("SubjectConsumerDecrement") == 0);
                break;
        }
        
    }
    //test property group
    //SubjectConsumerIncrementPropertyDescription cons_desc;
    //common::property::PropertyGroupSDWrapper group_ref_wrapper(CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(common::property::PropertyGroup, cons_desc));
    std::cout << trigger_environment.serialize()->getJSONString() << std::endl;
    return true;
}
