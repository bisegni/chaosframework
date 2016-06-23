/*
 *	ScriptDataAccess.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__D7C2C2C_991D_4C87_996D_74166C62D9A4_ScriptDataAccess_h
#define __CHAOSFramework__D7C2C2C_991D_4C87_996D_74166C62D9A4_ScriptDataAccess_h

#include "../persistence.h"

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                //!data access for script management
                class ScriptDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                    
                public:
                    DECLARE_DA_NAME
                    
                    //! default constructor
                    ScriptDataAccess();
                    
                    //!default destructor
                    ~ScriptDataAccess();
                    
                    //! Insert a new script in the database
                    /*!
                     \param Script that describe the script entry
                     */
                    virtual int insertNewScript(chaos::service_common::data::script::Script& new_Script) = 0;
                    
                    //! udate the script content
                    /*!
                     the script is identified using the base description
                     */
                    virtual int updateScript(chaos::service_common::data::script::Script& script) = 0;
                    
                    //!Perform a search on script entries
                    /*!
                     perform a simple search on node filtering on type
                     \param script_list the found elemento for current page
                     \param search_string is the search string
                     \param start_sequence_id is identified the sequence after wich we need to search
                     \param page_length is the maximum number of the element to return
                     */
                    virtual int searchScript(chaos::service_common::data::script::ScriptBaseDescriptionListWrapper& script_list,
                                             const std::string& search_string,
                                             uint64_t start_sequence_id,
                                             uint32_t page_length) = 0;
                    
                    //! add a new instance to the script
                    virtual int addScriptInstance(const uint64_t seq,
                                                  const std::string& script_name,
                                                  const std::string& instance_name) = 0;
                    
                    //! remove an instance to the script
                    virtual int removeScriptInstance(const uint64_t seq,
                                                     const std::string& script_name,
                                                     const std::string& instance_name) = 0;
                    
                    //!Perform a search on script entries
                    /*!
                     perform a simple search on node filtering on type
                     \param instance_list the found element for current page
                     \param is the name of the script for wich we need the isntance
                     \param search_string is the search string
                     \param start_sequence_id is identified the sequence after wich we need to search
                     \param page_length is the maximum number of the element to return
                     */
                    virtual int searchScriptInstance(std::vector<chaos::service_common::data::node::NodeInstance>& instance_list,
                                                     const std::string& script_name,
                                                     const std::string& search_string,
                                                     uint64_t start_sequence_id,
                                                     uint32_t page_length) = 0;
                    
                    //!Load a fulls cript information
                    /*!
                     \param script_base_description the base infromation to retrieve the script
                     \param script the script strucutre filled with all script infromation
                     \param load_source_code specify if we need to load all script source code
                     */
                    virtual int loadScript(const uint64_t unique_id,
                                           const std::string& name,
                                           chaos::service_common::data::script::Script& script,
                                           bool load_source_code = false) = 0;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__D7C2C2C_991D_4C87_996D_74166C62D9A4_ScriptDataAccess_h */
