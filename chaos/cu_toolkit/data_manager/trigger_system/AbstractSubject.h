/*
 *	AbstractSubject.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__F95AD0C_755D_4EE8_8709_A0118CE00CAB_AbstractSubjectReceiver_h
#define __CHAOSFramework__F95AD0C_755D_4EE8_8709_A0118CE00CAB_AbstractSubjectReceiver_h

#include <chaos/cu_toolkit/data_manager/trigger_system/trigger_system_types.h>

#include <string>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                
                //! present the abstraction of an event receiver
                /*!
                 Event receiver is the subject of event->consumer lifecicle. An event
                 receiver receive event of a determinated type and permit to 
                 attach to it some consumer for a determinated event
                 */
                class AbstractSubject {
                    const std::string receiver_name;
                    const std::string receiver_uuid;
                    
                public:
                    AbstractSubject(const std::string& _recevier_name);
                    virtual ~AbstractSubject();
                    
                    const std::string& getSubjectUUID() const;
                    
                    const std::string& getSubjectName() const;
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__F95AD0C_755D_4EE8_8709_A0118CE00CAB_AbstractSubject_h */
