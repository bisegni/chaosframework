/*
 *	TestEvent.h
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

#ifndef __CHAOSFramework_F0DCD41F_00FB_479F_A2F9_DB04B820DD8F_TestEvent_h
#define __CHAOSFramework_F0DCD41F_00FB_479F_A2F9_DB04B820DD8F_TestEvent_h

#include <chaos/cu_toolkit/data_manager/trigger_system/trigger_system.h>

using namespace chaos::cu::data_manager::trigger_system;

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
            
            class SubjectConsumer:
            public AbstractConsumer {
            public:
                SubjectConsumer(const std::string& name,
                                const std::string& description);
                ~SubjectConsumer();
                virtual ConsumerResult consumeEvent(TriggerDataEventType event_type,
                                                    TriggeredData& trigger_data) = 0;
            };
            //---------------------------------
            
            class SubjectEvent:
            public AbstractEvent {
            protected:
                virtual ConsumerResult _executeConsumerOnTarget(Subject *subject_instance,
                                                                SubjectConsumer *consumer_instance) = 0;
            public:
                SubjectEvent(const std::string& _event_name,
                             const std::string& _event_description,
                             const TriggerDataEventType _type);
                
                ConsumerResult executeConsumerOnTarget(AbstractSubject *subject_instance,
                                                       AbstractConsumer *consumer_instance);
            };
            
            //---------------------------------
            //create two event
            CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(SubjectEventOne,
                                                    SubjectEvent)
            CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY("offset", "amount of int's added to 1", chaos::DataType::TYPE_INT32);
            CHAOS_TRIGGER_CONSUMER_ADD_DEFINITION_1P(SubjectEventOne,
                                                     SubjectEvent,
                                                     "Signal for int increment event",
                                                     kTriggerDataEventTypeOne)
            ConsumerResult _executeConsumerOnTarget(Subject *subject_instance,
                                                    SubjectConsumer *consumer_instance);
            CHAOS_TRIGGER_CONSUMER_CLOSE_DEFINITION()
            
            
            CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(SubjectEventTwo,
                                                    SubjectEvent)
            CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY("offset", "amount of int's added to 1", chaos::DataType::TYPE_INT32);
            CHAOS_TRIGGER_CONSUMER_ADD_DEFINITION_1P(SubjectEventTwo,
                                                     SubjectEvent,
                                                     "Signal for int increment event",
                                                     kTriggerDataEventTypeTwo)
            ConsumerResult _executeConsumerOnTarget(Subject *subject_instance,
                                                    SubjectConsumer *consumer_instance);
            CHAOS_TRIGGER_CONSUMER_CLOSE_DEFINITION()
            //---------------------------------
            //create two consumer
            
            CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(SubjectConsumerIncrement,
                                                    SubjectConsumer)
            CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY("offset", "good property", chaos::DataType::TYPE_INT32);
            CHAOS_TRIGGER_CONSUMER_ADD_DEFINITION(SubjectConsumerIncrement,
                                                  SubjectConsumer,
                                                  "Preform integer increment")
            //we need to declare the custom consumer event
            ConsumerResult consumeEvent(TriggerDataEventType event_type,
                                        TriggeredData& trigger_data);
            CHAOS_TRIGGER_CONSUMER_CLOSE_DEFINITION()
            
            
            CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(SubjectConsumerDecrement,
                                                    SubjectConsumer)
            CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY("offset", "good property", chaos::DataType::TYPE_INT32);
            CHAOS_TRIGGER_CONSUMER_ADD_DEFINITION(SubjectConsumerDecrement,
                                                  SubjectConsumer,
                                                  "Preform integer decrement")
            ConsumerResult consumeEvent(TriggerDataEventType event_type,
                                        TriggeredData& trigger_data);
            CHAOS_TRIGGER_CONSUMER_CLOSE_DEFINITION()
            
            //---------------------------------
            
            class SubjectTriggerEnviroment:
            public RegisterEventConsumerEnvironment<SubjectEvent, SubjectConsumer, Subject, TriggerDataEventType> {
            public:
                SubjectTriggerEnviroment():
                RegisterEventConsumerEnvironment<SubjectEvent, SubjectConsumer, Subject, TriggerDataEventType>("SubjectName"){
                    
                    registerEventClass<SubjectEventOnePropertyDescription>();
                    registerEventClass<SubjectEventTwoPropertyDescription>();
                    
                    registerConsumerClass<SubjectConsumerIncrementPropertyDescription>();
                    registerConsumerClass<SubjectConsumerDecrementPropertyDescription>();
                }
                ~SubjectTriggerEnviroment(){}
            };
            
            //---------------------------------
            class TestEvent {
            public:
                TestEvent();
                ~TestEvent();
                bool test();
            };
        }
    }
}

#endif /* __CHAOSFramework_F0DCD41F_00FB_479F_A2F9_DB04B820DD8F_TestEvent_h */
