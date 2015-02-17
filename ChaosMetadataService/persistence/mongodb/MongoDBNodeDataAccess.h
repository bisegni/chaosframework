/*
 *	MongoDBNodeDataAccess.h
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
#ifndef __CHAOSFramework__MongoDBNodeDataAccess__
#define __CHAOSFramework__MongoDBNodeDataAccess__

#include "MongoDBAccessor.h"
#include "../data_access/NodeDataAccess.h"
#include <chaos/common/utility/ObjectInstancer.h>
namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for producer manipulation data
                class MongoDBNodeDataAccess:
                public data_access::NodeDataAccess,
                protected MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    friend class INSTANCER_P1(MongoDBNodeDataAccess, AbstractDataAccess, const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>&);
                protected:
                    MongoDBNodeDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBNodeDataAccess();
                public:
                    //inherited method
                    int insertNewNode(chaos::common::data::CDataWrapper& node_description);
                    
                    //! update the node updatable feature
                    int updateNode(chaos::common::data::CDataWrapper& node_description);
                    
                    // inherited method
                    int checkNodePresence(const std::string& node_unique_id,
                                          bool& presence);
                    
                    //! inherited method
                    int deleteNode(const std::string& node_unique_id);
                };

                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBUnitServerDataAccess__) */
