/*
 *	NodeDataAccess.h
 *	!CHOAS
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

#ifndef __CHAOSFramework__NodeDataAccess__
#define __CHAOSFramework__NodeDataAccess__

#include "../AbstractDataAccess.h"

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                /*!
                 Perform the basic operation on abstract node infomation
                 */
                class NodeDataAccess:
                public AbstractDataAccess {
                public:
                    //! default constructor
                    NodeDataAccess();
                    
                    //! defautl destructor
                    ~NodeDataAccess();
                    
                    //! insert a new device with name and property
                    /*!
                     the API receive a pack like the following one:
                     { 
                        //is the alias of the remote unit server
                        "unit_server_alias": "unit server alias",
                        
                        //are the control unit that are hosted on remote server
                        "unit_server_cu_alias": [ "Control Unit Class", "Control Unit Class" ],
                     
                        // is the rpc address where server respond
                        "cu_instance_net_address": "unit server RPC address host:port"
                     }
                    \param unit_server_description unit server key,value description
                     */
                    virtual int insertNewNode(chaos::common::data::CDataWrapper& node_description) = 0;
                    
                    //! check if a unit server identified by unique id is preset
                    /*!
                     \param unit_server_alias unit server unique id
                     \param presence = true if the unit server is present
                     */
                    virtual int checkNodePresence(const std::string& node_unique_id,
                                                  bool& presence) = 0;
                    
                    //! delete the node description
                    virtual int deleteNode(const std::string& node_unique_id) = 0;
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UnitServerDataAccess__) */
