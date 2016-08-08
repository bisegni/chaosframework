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

#include <string>

namespace chaos {
    namespace common {
        namespace trigger {
            
            //!base class for an algorith that can consume an event
            /*!
             An event consumer realize some fast check or converion
             on event data.
             */
            class AbstractConsumer:
            public chaos::common::property::PropertyGroup {
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
                
                void updateProperty(const PropertyGroup& property_group);
            };
        }
    }
}

#endif /* __CHAOSFramework__A3F7AA7_069E_4235_B2D7_F8944A455374_AbstractConsumer_h */
