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
            
            typedef ChaosSharedPtr<TriggeredData> TriggeredDataShrdPtr;
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
