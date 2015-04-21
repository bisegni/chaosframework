/*
 *	NodeDataAccess.h
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

#ifndef __CHAOSFramework__NodeDataAccess__
#define __CHAOSFramework__NodeDataAccess__

#include "../persistence.h"

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {

                /*!
                 Perform the basic operation on abstract node infomation
                 */
                class NodeDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                public:
                    DECLARE_DA_NAME
                        //! default constructor
                    NodeDataAccess();

                        //! defautl destructor
                    ~NodeDataAccess();

                        //! return the whole node description
                    /*!
                     return the whole node description as is on database
                     \param node_unique_id the unique id of the node
                     \param node_description the handler to the node description
                     */
                    virtual int getNodeDescription(const std::string& node_unique_id,
                                                   chaos::common::data::CDataWrapper **node_description) = 0;

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
                    virtual int checkNodePresence(bool& presence,
                                                  const std::string& node_unique_id,
                                                  const std::string& node_unique_type = std::string()) = 0;

                        //! delete the node description
                    /*
                     The API delete the node description.
                     */
                    virtual int deleteNode(const std::string& node_unique_id,
                                           const std::string& node_type = std::string()) = 0;

                        //!Make simple paged query on node
                    /*!
                     perform a simple search on node filtering on type
                     \param result the handle for the reuslt data (is allcoated only if the search has been done with success)
                     \param criteria the stirng that filter the node (susing * semantic)
                     \param search_type the type of the nodes where search
                     \param last_unique_id is the start of the search page to retrieve
                     */
                    virtual int searchNode(chaos::common::data::CDataWrapper **result,
                                           const std::string& criteria,
                                           uint32_t search_type,
                                           uint32_t last_unique_id,
                                           uint32_t page_length = 50) = 0;
                };

            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UnitServerDataAccess__) */
