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

#include <chaos/cu_toolkit/data_manager/trigger_system/trigger_system_types.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/AbstractEventConsumer.h>

#include <boost/shared_ptr.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <string>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                
                
                //!forward decalration
                struct TriggerEventMIExstractName;
                struct TriggerEventMIExstractCode;
                
                
                //! define a base event description that can trigger some executio
                /*!
                 an event is defined by a name and a code and they are unique
                 within the data broker instance
                 */
                class TriggerEvent {
                    friend struct TriggerEventMIExstractName;
                    friend struct TriggerEventMIExstractCode;
                    const std::string    event_name;
                    const unsigned int   event_code;
                    
                public:
                    TriggerEvent(const std::string& _event_name,
                                 const unsigned int _event_code);
                    
                    const std::string& getName() const;
                    const unsigned int& getCode() const;
                };
                
                
                //!trigger event shared pointer
                typedef boost::shared_ptr<TriggerEvent> TriggerEventShrdPtr;
                
                //!multi index key extractor
                struct TriggerEventMIExstractName {
                    typedef std::string result_type;
                    const result_type &operator()(const TriggerEventShrdPtr &p) const;
                };
                
                struct TriggerEventMIExstractCode {
                    typedef unsigned int result_type;
                    const result_type &operator()(const TriggerEventShrdPtr &p) const;
                };
                
                
                //tag
                struct TEMITagCode{};
                struct TEMITagName{};
                
                //multi-index set
                typedef boost::multi_index_container<
                TriggerEventShrdPtr,
                boost::multi_index::indexed_by<
                boost::multi_index::ordered_non_unique<boost::multi_index::tag<TEMITagCode>,  TriggerEventMIExstractCode>,
                boost::multi_index::hashed_non_unique<boost::multi_index::tag<TEMITagName>,  TriggerEventMIExstractName>
                >
                > TriggerEventContainer;
                
                //!index for container
                //code
                typedef boost::multi_index::index<TriggerEventContainer, TEMITagCode>::type             TECodeIndex;
                typedef boost::multi_index::index<TriggerEventContainer, TEMITagCode>::type::iterator   TECodeIndexIterator;
                //name
                typedef boost::multi_index::index<TriggerEventContainer, TEMITagName>::type             TENameIndex;
                typedef boost::multi_index::index<TriggerEventContainer, TEMITagName>::type::iterator   TENameIndexIterator;
                
                
                
            }
        }
    }
}

#endif /* __CHAOSFramework__1DFC893_F993_4167_A7E9_D5E470648BA8_event_system_types_h */
