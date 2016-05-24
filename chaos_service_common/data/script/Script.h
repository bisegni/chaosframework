/*
 *	Script.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__B6FD4CC_B9C0_439B_9FD7_53540B34EC15_Script_h
#define __CHAOSFramework__B6FD4CC_B9C0_439B_9FD7_53540B34EC15_Script_h

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/TemplatedDataContainer.h>

namespace chaos {
    namespace service_common {
        namespace data {
            namespace script {
                
#define SBD_NAME        "name"
#define SBD_DESCRIPTION "description"
                
                
                //! The description of a script
                struct ScriptBaseDescription {
                    uint64_t unique_id;
                    std::string name;
                    std::string description;
                    
                    ScriptBaseDescription();
                    ScriptBaseDescription(const ScriptBaseDescription& copy_src);
                    
                    ScriptBaseDescription& operator=(ScriptBaseDescription const &rhs);
                };
                
                //!helper for create or read the script description
                class ScriptBaseDescriptionHelper:
                public TemplatedDataContainer<ScriptBaseDescription>{
                public:
                    ScriptBaseDescriptionHelper();
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data);
                    std::auto_ptr<chaos::common::data::CDataWrapper> serialize(const uint64_t sequence = 0);
                };
                
                //!a list of a script base information usefullt for search operation
                CHAOS_DEFINE_VECTOR_FOR_TYPE(ScriptBaseDescription, ScriptBaseDescriptionList)
             
#define SBD_SCRIPT_CONTENT "content"

                
                //! Full script description
                struct Script {
                    //!base script
                    ScriptBaseDescription script_description;
                    //! the sourc ecode of the script
                    std::string script_content;
                    
                    Script();
                    Script(const Script& copy_src);
                    
                    Script& operator=(Script const &rhs);
                };
                
                //!heper for script class
                class ScriptHelper:
                public TemplatedDataContainer<Script> {
                    ScriptBaseDescriptionHelper sdh;
                public:
                    ScriptHelper();
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data);
                    std::auto_ptr<chaos::common::data::CDataWrapper> serilize(const uint64_t sequence = 0);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__B6FD4CC_B9C0_439B_9FD7_53540B34EC15_Script_h */
