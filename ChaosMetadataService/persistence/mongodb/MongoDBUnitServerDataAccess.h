/*
 *	MongoDBUnitServerDataAccess.h
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
#ifndef __CHAOSFramework__MongoDBUnitServerDataAccess__
#define __CHAOSFramework__MongoDBUnitServerDataAccess__

#include "MongoDBAccessor.h"
#include "../data_access/UnitServerDataAccess.h"
#include <chaos/common/utility/ObjectInstancer.h>
namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for producer manipulation data
                class MongoDBUnitServerDataAccess:
                public data_access::UnitServerDataAccess,
                protected MongoDBAccessor {
                    friend class INSTANCER_P1(MongoDBUnitServerDataAccess, AbstractDataAccess, const boost::shared_ptr<MongoDBHAConnectionManager>&);
                protected:
                    MongoDBUnitServerDataAccess(const boost::shared_ptr<MongoDBHAConnectionManager>& _connection);
                    ~MongoDBUnitServerDataAccess();
                public:
                    //inherited method
                    int insertNewUnitServer(chaos::common::data::CDataWrapper& unit_server_description);
                    
                    // inherited method
                    int checkUnitServerPresence(const std::string& unit_server_alias,
                                                bool& presence);
                    
                    //! inherited method
                    int updateUnitServer(chaos::common::data::CDataWrapper& unit_server_description);
                };

                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBUnitServerDataAccess__) */
