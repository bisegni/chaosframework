/*
 *	UnitServerDataAccess.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__UnitServerDataAccess__
#define __CHAOSFramework__UnitServerDataAccess__

#include "../persistence.h"

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                class UnitServerDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                public:
                    DECLARE_DA_NAME

                    //! default constructor
                    UnitServerDataAccess();
                    
                    //!default destructor
                    ~UnitServerDataAccess();
                    
                    //! Insert the unit server information
                    /*!
                     Insert a new Unit Server node. After calling the NodeDataAccess method for insert new node it update the created node with the value of the
                     keys from the namespace chaos::UnitServerNodeDefinitionKey:
                     UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS(mandatory) that is a vector of string that represent the hosted unit server control unit class.
                     
                     \param unit_server_description unit server key,value description
                     */
                    virtual int insertNewUS(chaos::common::data::CDataWrapper& unit_server_description) = 0;
                    
                    //!check if the unit server is present
                    /*!
                     Call directly the node data access to check if the node exist
                     */
                    virtual int checkUSPresence(const std::string& unit_server_alias, bool& presence) = 0;
                    
                    //! Update the unit server information
                    /*!
                     Perform the update operation on the unit server description, the DA frist need to call the update of the node data
                     access to update node information next needs to update the unit server custom property from the namespace 
                     chaos::UnitServerNodeDefinitionKey.
                     UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS(mandatory)
                     
                     \param unit_server_description unit server key,value description
                     */
                    virtual int updateUS(chaos::common::data::CDataWrapper& unit_server_description) = 0;
                    
                    //! delete a unit server
                    virtual int deleteUS(const std::string& unit_server_unique_id) = 0;
                };
                
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UnitServerDataAccess__) */
