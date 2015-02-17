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
                    
                    //! insert a new node
                    /*!
                     The API receive a data pack with one or more key listed in chaos::NodeDefinitionKey namespace,
                     the mandatory key are:
                     NODE_UNIQUE_ID,
                     NODE_TYPE [values need to be one of the @chaos::NodeType],
                     NODE_RPC_ADDR,
                     NODE_RPC_DOMAIN,
                     NODE_TIMESTAMP.
                     \param node_description the node description
                     \return 0 for no error otherwhise a negative value
                     */
                    virtual int insertNewNode(chaos::common::data::CDataWrapper& node_description) = 0;
                    
                    //! update the node information
                    /*!
                     The API receive a data pack with one or more key listed in chaos::NodeDefinitionKey namespace.
                     The mandatory key are the NODE_UNIQUE_ID need to find the description and the only updateable key are:
                     NODE_RPC_ADDR,
                     NODE_RPC_DOMAIN,
                     NODE_TIMESTAMP.
                     The NODE_TYPE key can't be updated
                     \param node_description the node description
                     \return 0 for no error otherwhise a negative value
                     */
                    virtual int updateNode(chaos::common::data::CDataWrapper& node_description) = 0;
                    
                    
                    //! check if a unit server identified by unique id is preset
                    /*!
                     The API check is the description of the node has been created. The variable represent the answer to the
                     request only if the result of the api is 0;
                     \param unit_server_alias unit server unique id
                     \param presence = true if the unit server is present
                     \return 0 for no error otherwhise a negative value
                     */
                    virtual int checkNodePresence(const std::string& node_unique_id,
                                                  bool& presence) = 0;
                    
                    //! delete the node description
                    /*
                     The API delete the node description.
                     */
                    virtual int deleteNode(const std::string& node_unique_id) = 0;
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UnitServerDataAccess__) */
