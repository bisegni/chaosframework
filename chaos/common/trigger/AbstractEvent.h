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
                
                const std::string   event_name;
                const std::string   event_description;
                const unsigned int  event_code;
                
            public:
                //! event constructor with default values
                AbstractEvent(const std::string& _event_name,
                              const std::string& _event_description,
                              const unsigned int _event_code):
                PropertyGroup(_event_name),
                event_name(_event_name),
                event_description(_event_description),
                event_code(_event_code){}

                
                const std::string& getEventName() const {
                    return event_name;
                }

                const std::string& getEventDescription() const {
                    return event_description;
                }

                const unsigned int& getEventCode() const {
                    return event_code;
                }

                
                chaos::common::property::PropertyGroup& getProperties() {
                    return *this;
                }

                
                virtual ConsumerResult executeConsumerOnTarget(SubjectImpl *subject_instance,
                                                               AbstractConsumer<EventType, SubjectImpl> *consumer_instance) = 0;
            };
        }
    }
}

#endif /* __CHAOSFramework__1DFC893_F993_4167_A7E9_D5E470648BA8_event_system_types_h */
