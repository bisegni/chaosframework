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
                    
                    //!Delete ascript and all instance related
                    /*!
                     \param script_base_description the base infromation to retrieve the script
                     \param script the script strucutre filled with all script infromation
                     */
                    virtual int deleteScript(const uint64_t unique_id,
                                             const std::string& name) = 0;
                    
                    
                    //!Return script associated to the execution pool list
                    /*!
                     \param pool_path a list of pool path
                     \param script_found the found script that belong to one or more execution pool path
                     \param max_result determinate how much instance need to return
                     */
                    virtual int getScriptForExecutionPoolPathList(const ChaosStringVector& pool_path,
                                                                  std::vector<chaos::service_common::data::script::ScriptBaseDescription>& script_found,
                                                                  uint64_t last_sequence_id = 0,
                                                                  uint32_t max_result = 10) = 0;
                    
                    //!Return all unscheduled instance for the script
                    /*!
                     \param script script description
                     \param instance_found_list the found instances that are not running
                     \param timeout the amount of milliseconds after wich an isntance is consider in timeout
                     \param max_result determinate how much instance need to return
                     */
                    virtual int getUnscheduledInstanceForJob(const chaos::service_common::data::script::ScriptBaseDescription& script,
                                                             ChaosStringVector& instance_found_list,
                                                             uint32_t timeout = 30000,
                                                             uint32_t max_result = 10) = 0;
                    
                    /*!
                     prenotate the instance to be executed on a unit server
                     \param instance_uid the uinque id of the instance
                     \param unit_server_parent is the unit server that wish to reserve that instance
                     \param timeout the amount of milliseconds after wich an isntance is consider in timeout
                     */
                    virtual int reserveInstanceForScheduling(bool& reserverd,
                                                             const std::string& instance_uid,
                                                             const std::string& unit_server_parent,
                                                             uint32_t timeout) = 0;
                    
                    //!copy the script dataset to an his instance
                    /*!
                     this is done because during the script load operation we need to ensure that meanwhile script is runinng, if
                     the script is modified in his dataset or content, the instance doens't need to have problem. So instance will have
                     a snapshop of script dataset at the load moment. API Check if the instance belogn to the script so it is
                     safe.
                     \param script, the base information that permit to find a script
                     \param script_instance the instance connected to the script
                     */
                    virtual int copyScriptDatasetAndContentToInstance(const chaos::service_common::data::script::ScriptBaseDescription& script,
                                                            const std::string& script_instance) = 0;
                    
                    //! make an heartbeat on the instance
                    /*!
                     the first hearttbeat for an instance and new unit server, this last is set as parent
                     mds consider an instance as runnit unitil hb is within the timeout delay. Pass out
                     the timeout the istance is considered not runnit so on the first exectuion pool
                     request it will be rescheduled.
                     */
                    virtual int instanceForUnitServerHeartbeat(const ChaosStringVector& script_instance_list,
                                                               const std::string& unit_server_parent,
                                                               uint32_t timeout) = 0;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__D7C2C2C_991D_4C87_996D_74166C62D9A4_ScriptDataAccess_h */
