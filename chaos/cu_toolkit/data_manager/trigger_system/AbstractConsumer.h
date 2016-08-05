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
#include <chaos/common/data/CDataVariant.h>

#include <chaos/cu_toolkit/data_manager/manipulation/DatasetEditor.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/trigger_system_types.h>

#include <string>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::common::data::CDataVariant, TriggerEventValues);
                
                //!base class for an algorith that can consume an event
                /*!
                 An event consumer realize some fast check or converion
                 on event data.
                 */
                class AbstractConsumer {
                    const std::string consumer_name;
                    const std::string consumer_description;
                    const std::string consumer_uuid;
                public:
                    AbstractConsumer(const std::string& _consumer_name,
                                     const std::string& _consumer_description);
                    virtual ~AbstractConsumer();
                    
                    const std::string& getConsumerUUID() const;
                    const std::string& getConsumerName() const;
                    const std::string& getConsumerDescription() const;
                };
             }
        }
    }
}

#endif /* __CHAOSFramework__A3F7AA7_069E_4235_B2D7_F8944A455374_AbstractConsumer_h */
