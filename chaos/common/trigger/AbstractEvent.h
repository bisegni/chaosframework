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

#include <boost/shared_ptr.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <string>

namespace chaos {
    namespace common {
        namespace trigger {
            
            //!forward decalration
            struct AbstractEventMIExstractName;
            struct AbstractEventMIExstractCode;
            
            //! define a base event description that can trigger some executio
            /*!
             an event is defined by a name and a code and they are unique
             within the data broker instance
             */
            class AbstractEvent:
            public chaos::common::property::PropertyGroup {
                friend struct AbstractEventMIExstractName;
                friend struct AbstractEventMIExstractCode;
                
                const std::string   event_name;
                const std::string   event_description;
                const unsigned int  event_code;
                
            public:
                //! event constructor with default values
                AbstractEvent(const std::string& _event_name,
                              const std::string& _event_description,
                              const unsigned int _event_code);
                
                const std::string& getEventName() const;
                const std::string& getEventDescription() const;
                const unsigned int& getEventCode() const;
                
                chaos::common::property::PropertyGroup& getProperties();
                
                virtual ConsumerResult executeConsumerOnTarget(AbstractSubject *subject_instance,
                                                               AbstractConsumer *consumer_instance) = 0;
            };
            
            
            //!trigger event shared pointer
            typedef boost::shared_ptr<AbstractEvent> AbstractEventShrdPtr;
            
            //!multi index key extractor
            struct AbstractEventMIExstractName {
                typedef std::string result_type;
                const result_type &operator()(const AbstractEventShrdPtr &p) const;
            };
            
            struct AbstractEventMIExstractCode {
                typedef unsigned int result_type;
                const result_type &operator()(const AbstractEventShrdPtr &p) const;
            };
            
            
            //tag
            struct EMITagCode{};
            struct EMITagName{};
            struct EMITagDomain{};
            
            //multi-index set
            typedef boost::multi_index_container<
            AbstractEventShrdPtr,
            boost::multi_index::indexed_by<
            boost::multi_index::ordered_non_unique<boost::multi_index::tag<EMITagCode>,  AbstractEventMIExstractCode>,
            boost::multi_index::hashed_non_unique<boost::multi_index::tag<EMITagName>,  AbstractEventMIExstractName>
            >
            > AbstractEventContainer;
            
            //!index for container
            //code
            typedef boost::multi_index::index<AbstractEventContainer, EMITagCode>::type                TECodeIndex;
            typedef boost::multi_index::index<AbstractEventContainer, EMITagCode>::type::iterator      TECodeIndexIterator;
            //name
            typedef boost::multi_index::index<AbstractEventContainer, EMITagName>::type                TENameIndex;
            typedef boost::multi_index::index<AbstractEventContainer, EMITagName>::type::iterator      TENameIndexIterator;
        }
    }
}

#endif /* __CHAOSFramework__1DFC893_F993_4167_A7E9_D5E470648BA8_event_system_types_h */
