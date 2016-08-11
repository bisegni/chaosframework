/*
 *	event_system_types.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 29/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__1DFC893_F993_4167_A7E9_D5E470648BA8_event_system_types_h
#define __CHAOSFramework__1DFC893_F993_4167_A7E9_D5E470648BA8_event_system_types_h

#include <chaos/common/property/PropertyGroup.h>

#include <chaos/common/trigger/trigger_types.h>
#include <chaos/common/trigger/AbstractConsumer.h>
#include <chaos/common/trigger/AbstractSubject.h>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <string>

namespace chaos {
    namespace common {
        namespace trigger {
            
            //! define a base event description that can trigger some executio
            /*!
             an event is defined by a name and a code and they are unique
             within the data broker instance
             */
            template<typename EventType, typename SubjectImpl>
            class AbstractEvent:
            public chaos::common::property::PropertyGroup {
                EventType event_code;
            public:
                
                //! event constructor with default values
                AbstractEvent(const std::string& _event_name,
                              const EventType _event_code):
                PropertyGroup(_event_name),
                event_code(_event_code){}
                
                const std::string& getEventName() const {
                    return PropertyGroup::getGroupName();
                }
                
                const EventType getEventCode() const {
                    return event_code;
                }
                
                
                chaos::common::property::PropertyGroup& getProperties() {
                    return *this;
                }
                
                
                ConsumerResult executeConsumerOnTarget(SubjectImpl *subject_instance,
                                                       AbstractConsumer<EventType, SubjectImpl> *consumer_instance) {
                    //check if consumer is enabled
                    if(consumer_instance->isEnabled() == false) return ConsumerResultOK;
                    
                    //we can run th consumer
                    return consumer_instance->consumeEvent(static_cast<EventType>(getEventCode()),
                                                           *subject_instance);
                }
            };
            
            //!forward decalration
            struct AbstractEventMIExstractName;
            struct AbstractEventMIExstractCode;
            
            class BaseEventInstancerDescription {
                friend struct AbstractEventMIExstractName;
                friend struct AbstractEventMIExstractCode;
            protected:
                const std::string   event_name;
                const std::string   event_description;
                const unsigned int  event_code;
            public:
                BaseEventInstancerDescription(const std::string& _event_name,
                                              const std::string& _event_description,
                                              const unsigned int _event_code):
                event_name(_event_name),
                event_description(_event_description),
                event_code(_event_code){}
                virtual ~BaseEventInstancerDescription(){}
                typedef boost::shared_ptr< BaseEventInstancerDescription > AbstractEventShrdPtr;
                
            };
            
            struct AbstractEventMIExstractName {
                typedef std::string result_type;
                const result_type &operator()(const BaseEventInstancerDescription::AbstractEventShrdPtr &p) const {
                    return p->event_name;
                }
            };
            
            struct AbstractEventMIExstractCode {
                typedef unsigned int result_type;
                const result_type &operator()(const BaseEventInstancerDescription::AbstractEventShrdPtr &p) const {
                    return p->event_code;
                }
            };
            
            //!consumer description
            template<typename EventType,
            typename SubjectImpl>
            class EventInstancerDescription:
            public BaseEventInstancerDescription,
            public property::PropertyGroup {
            public:
                typedef boost::shared_ptr< EventInstancerDescription<EventType, SubjectImpl> > EventInstancerShrdPtr;
            public:
                //tag
                struct TriggerEventTagCode{};
                struct TriggerEventTagName{};
                //multi-index set
                typedef boost::multi_index_container<
                EventInstancerShrdPtr,
                boost::multi_index::indexed_by<
                boost::multi_index::ordered_non_unique<boost::multi_index::tag<TriggerEventTagCode>,  AbstractEventMIExstractCode>,
                boost::multi_index::hashed_non_unique<boost::multi_index::tag<TriggerEventTagName>,  AbstractEventMIExstractName>
                >
                > EventDescriptionContainer;
                
                //!index for container
                //code
                typedef typename boost::multi_index::index<EventDescriptionContainer, TriggerEventTagCode>::type                TECodeIndex;
                typedef typename boost::multi_index::index<EventDescriptionContainer, TriggerEventTagCode>::type::iterator      TECodeIndexIterator;
                //name
                typedef typename boost::multi_index::index<EventDescriptionContainer, TriggerEventTagName>::type                TENameIndex;
                typedef typename boost::multi_index::index<EventDescriptionContainer, TriggerEventTagName>::type::iterator      TENameIndexIterator;
                
                typedef AbstractEvent<EventType, SubjectImpl > ConcreteEvent;
                
                EventInstancerDescription(const std::string& name,
                                          const std::string& description,
                                          const EventType   event_type):
                PropertyGroup(name),
                BaseEventInstancerDescription(name,
                                              description,
                                              event_type){}
                
                const std::string& getEventName() const {
                    return event_name;
                }
                
                const std::string getEventDescription() const {
                    return event_description;
                }
                
                const EventType getEventType() const {
                    return static_cast<EventType>(event_code);
                }
                
                ConcreteEvent* getInstance() {
                    ConcreteEvent* new_instance = new ConcreteEvent(event_name, static_cast<EventType>(event_code));
                    //copy property from twhi group
                    property::PropertyGroup *pg_instance = dynamic_cast<property::PropertyGroup*>(new_instance);
                    if(pg_instance) {
                        //copy all properties from description to event
                        new_instance->copyPropertiesFromGroup(*this,
                                                              true);
                    }
                    return new_instance;
                }
            };
#define CHAOS_TRIGGER_EVENT_OPEN_DESCRIPTION(impl, description, event_code, EventType, SubjectImpl)\
class impl ## TriggerEventDescription:\
public chaos::common::trigger::EventInstancerDescription< EventType, SubjectImpl> {\
public:\
impl ## TriggerEventDescription():\
chaos::common::trigger::EventInstancerDescription< EventType, SubjectImpl >(#impl, description, event_code) {
            
#define CHAOS_TRIGGER_EVENT_ADD_PROPERTY(name, desc, type)\
addProperty(name, desc, type);
            
#define CHAOS_TRIGGER_EVENT_CLOSE_DESCRIPTION()\
}\
};
        }
    }
}

#endif /* __CHAOSFramework__1DFC893_F993_4167_A7E9_D5E470648BA8_event_system_types_h */
