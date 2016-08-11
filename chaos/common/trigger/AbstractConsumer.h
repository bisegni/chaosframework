/*
 *	AbstractConsumer.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 28/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__A3F7AA7_069E_4235_B2D7_F8944A455374_AbstractConsumer_h
#define __CHAOSFramework__A3F7AA7_069E_4235_B2D7_F8944A455374_AbstractConsumer_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/property/PropertyGroup.h>
#include <chaos/common/trigger/trigger_types.h>
#include <chaos/common/utility/UUIDUtil.h>

#include <string>

#define CHAOS_TRIGGER_CONSUMER_PROPERTY_ENABLED "enabled"

namespace chaos {
    namespace common {
        namespace trigger {
            
            //!base class for an algorith that can consume an event
            /*!
             An event consumer realize some fast check or converion
             on event data.
             */
            template<typename EventType, typename SubjectImpl>
            class AbstractConsumer:
            public chaos::common::property::PropertyGroup {
                const std::string consumer_uuid;
                //!reference to CHAOS_TRIGGER_CONSUMER_PROPERTY_ENABLED property
                chaos::common::property::PropertyDescription& enabled_prop_ref;
            public:
               // AbstractConsumer(const std::string& consumer_name):
               // PropertyGroup(consumer_name),
               // consumer_uuid(utility::UUIDUtil::generateUUIDLite()),
               // enabled_prop_ptr(NULL){}
                
                AbstractConsumer(const chaos::common::property::PropertyGroup& src_pg):
                PropertyGroup(src_pg),
                consumer_uuid(utility::UUIDUtil::generateUUIDLite()),
                enabled_prop_ref(PropertyGroup::getProperty(CHAOS_TRIGGER_CONSUMER_PROPERTY_ENABLED)){}
                
                virtual ~AbstractConsumer(){}
                
                const std::string& getConsumerUUID() const {
                    return consumer_uuid;
                }
                
                const std::string& getConsumerName() const {
                    return PropertyGroup::getGroupName();
                }
                
                void updateProperty(const PropertyGroup& property_group) {
                    //we need to reaset all values
                    for (property::MapPropertiesConstIterator it = property_group.getAllProperties().begin(),
                         end = property_group.getAllProperties().end();
                         it != end;
                         it++) {
                        PropertyGroup::setPropertyValue(it->first, it->second.getPropertyValue());
                    }
                }
                
                bool isEnabled() {
                    return enabled_prop_ref.getPropertyValue().asBool();
                }
                
                virtual ConsumerResult consumeEvent(EventType event_type,
                                                    SubjectImpl& trigger_data) = 0;
            };
            
            
            
            
            template<typename EventType,
            typename SubjectImpl>
            class ConsumerInstancerDescription:
            public chaos::common::property::PropertyGroup {
                const std::string consumer_description;
            protected:
                ConsumerInstancerDescription(const std::string& name,
                                             const std::string& description):
                consumer_description(description),
                PropertyGroup(name){}
                
            public:
                typedef AbstractConsumer<EventType, SubjectImpl > ConcreteConsumer;
                typedef boost::shared_ptr< ConcreteConsumer > ConsumerShrdPtr;
                typedef boost::shared_ptr< ConsumerInstancerDescription<EventType, SubjectImpl> > ConsumerInstancerShrdPtr;
                
                const std::string& getConsumerName() const {
                    return PropertyGroup::getGroupName();
                }
                
                const std::string& getConsumerDescription() const {
                    return consumer_description;
                }
                
                virtual ConcreteConsumer* getInstance() = 0;
            };
            
            template<typename impl,typename EventType, typename SubjectImpl>
            class ImplementationConsumerInstancerDescription:
            public ConsumerInstancerDescription<EventType, SubjectImpl> {
            public:
                ImplementationConsumerInstancerDescription(const std::string& name,
                                                           const std::string& description):
                ConsumerInstancerDescription<EventType, SubjectImpl>(name,
                                                                     description){}
                
                typename ConsumerInstancerDescription<EventType, SubjectImpl>::ConcreteConsumer* getInstance() {
                    typename ConsumerInstancerDescription<EventType, SubjectImpl>::ConcreteConsumer* new_instance = new impl(*this);
                    //copy property from twhi group
                    //chaos::common::property::PropertyGroup *pg_instance = dynamic_cast<chaos::common::property::PropertyGroup*>(new_instance);
                    //if(pg_instance) {
                        //copy all properties from description to consumer
                      //  new_instance->copyPropertiesFromGroup(*this,
                         //                                     true);
                    //}
                    return new_instance;
                }
            };
            
#define CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(impl, description, EventType, SubjectImpl)\
class impl:\
public chaos::common::trigger::AbstractConsumer<EventType, SubjectImpl> {\
public:\
impl(const chaos::common::property::PropertyGroup& src_pg):AbstractConsumer<EventType, SubjectImpl>(src_pg){}\
chaos::common::trigger::ConsumerResult consumeEvent(EventType event_type, SubjectImpl& trigger_data);};\
\
class impl ## TriggerConsumerDescription:\
public chaos::common::trigger::ImplementationConsumerInstancerDescription< impl, EventType, SubjectImpl> {\
public:\
impl ## TriggerConsumerDescription():\
chaos::common::trigger::ImplementationConsumerInstancerDescription< impl, EventType, SubjectImpl >(#impl, description) {\
addProperty(CHAOS_TRIGGER_CONSUMER_PROPERTY_ENABLED, "Enable the consumer activity", chaos::DataType::TYPE_BOOLEAN);\
setPropertyValue(CHAOS_TRIGGER_CONSUMER_PROPERTY_ENABLED, chaos::common::data::CDataVariant(true));
            
#define CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY(name, desc, type)\
addProperty(name, desc, type);
            
#define CHAOS_TRIGGER_CONSUMER_CLOSE_DESCRIPTION()\
}\
};
            
        }
    }
}
#endif /* __CHAOSFramework__A3F7AA7_069E_4235_B2D7_F8944A455374_AbstractConsumer_h */
