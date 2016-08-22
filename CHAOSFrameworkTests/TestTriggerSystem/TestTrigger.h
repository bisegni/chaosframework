/*
 *	TestTrigger.h
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

#ifndef __CHAOSFramework_F0DCD41F_00FB_479F_A2F9_DB04B820DD8F_TestTrigger_h
#define __CHAOSFramework_F0DCD41F_00FB_479F_A2F9_DB04B820DD8F_TestTrigger_h

#include <chaos/common/trigger/trigger.h>

using namespace chaos::common::trigger;

namespace chaos {
    namespace test {
        namespace trigger_system {
            //---------------------------------
            typedef enum TriggerDataEventType {
                kTriggerDataEventTypeOne,
                kTriggerDataEventTypeTwo
            }TriggerDataEventType;
            //---------------------------------
            class TriggeredData {
            public:
                int data_value;
                TriggeredData(int _new_value);
            };
            
            typedef boost::shared_ptr<TriggeredData> TriggeredDataShrdPtr;
            //---------------------------------
            
            class Subject:
            public AbstractSubject {
            public:
                TriggeredDataShrdPtr subject_data;
                Subject(const TriggeredDataShrdPtr& _subject_data);
            };
            
            //---------------------------------
            //create two event
            CHAOS_TRIGGER_EVENT_OPEN_DESCRIPTION(SubjectEventOne,
                                                 "Signal for int increment event",
                                                 kTriggerDataEventTypeOne,
                                                 TriggerDataEventType,
                                                 Subject)
            CHAOS_TRIGGER_EVENT_ADD_PROPERTY("offset", "amount of int's added to 1", chaos::DataType::TYPE_INT32);
            CHAOS_TRIGGER_EVENT_CLOSE_DESCRIPTION()
            
            CHAOS_TRIGGER_EVENT_OPEN_DESCRIPTION(SubjectEventTwo,
                                                 "Signal for int increment event",
                                                 kTriggerDataEventTypeTwo,
                                                 TriggerDataEventType,
                                                 Subject)
            CHAOS_TRIGGER_EVENT_ADD_PROPERTY("offset", "amount of int's added to 1", chaos::DataType::TYPE_INT32);
            CHAOS_TRIGGER_EVENT_CLOSE_DESCRIPTION()
            
            //---------------------------------
            //create two consumer
            CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(SubjectConsumerIncrement,
                                                    "Preform integer increment",
                                                    TriggerDataEventType,
                                                    Subject)
            
            CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY("offset", "good property", chaos::DataType::TYPE_INT32);
            CHAOS_TRIGGER_CONSUMER_CLOSE_DESCRIPTION()
            
            CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(SubjectConsumerDecrement,
                                                    "Preform integer decrement",
                                                    TriggerDataEventType,
                                                    Subject)
            CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY("offset", "good property", chaos::DataType::TYPE_INT32);
            CHAOS_TRIGGER_CONSUMER_CLOSE_DESCRIPTION()
            
            //---------------------------------
            
            class SubjectTriggerEnviroment:
            public TriggerExecutionEnviroment<Subject, TriggerDataEventType> {
            public:
                SubjectTriggerEnviroment():
                TriggerExecutionEnviroment<Subject, TriggerDataEventType>("SubjectName"){
                    
                    registerEventClass<SubjectEventOneTriggerEventDescription>();
                    registerEventClass<SubjectEventTwoTriggerEventDescription>();
                    
                    registerConsumerClass<SubjectConsumerIncrementTriggerConsumerDescription>();
                    registerConsumerClass<SubjectConsumerDecrementTriggerConsumerDescription>();
                }
                ~SubjectTriggerEnviroment(){}
            };
            
            //---------------------------------
            class TestTrigger {
            public:
                TestTrigger();
                ~TestTrigger();
                bool test();
            };
        }
    }
}

#endif /* __CHAOSFramework_F0DCD41F_00FB_479F_A2F9_DB04B820DD8F_TestTrigger_h */
