/*
 *	MongoDBUnitServerDataAccess.h
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
#ifndef __CHAOSFramework__MongoDBUnitServerDataAccess__
#define __CHAOSFramework__MongoDBUnitServerDataAccess__


#include "MongoDBNodeDataAccess.h"
#include "../data_access/UnitServerDataAccess.h"

#include <chaos/common/utility/ObjectInstancer.h>

#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
	namespace metadata_service {
		namespace persistence {
			namespace mongodb {
				
				//forward declaration
				class MongoDBPersistenceDriver;
				
                //! Data Access for producer manipulation data
				class MongoDBUnitServerDataAccess:
				public data_access::UnitServerDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    
                    MongoDBNodeDataAccess *node_data_access = NULL;
				protected:
                    MongoDBUnitServerDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
					~MongoDBUnitServerDataAccess();
				public:
                    //inherited method
                    int insertNewUS(chaos::common::data::CDataWrapper& unit_server_description);
                    
                    //inherited method
                    int updateUS(chaos::common::data::CDataWrapper& unit_server_description);
                    
                    //inherited method
                    int checkPresence(const std::string& unit_server_unique_id,
                                      bool& presence);

                    //inherited method
                    int deleteUS(const std::string& unit_server_unique_id);

                    //inherited method
                    int getDescription(const std::string& unit_server_uid,
                                       chaos::common::data::CDataWrapper **unit_server_description);
				};
			}
		}
	}
}
#endif /* defined(__CHAOSFramework__MongoDBProducerDataAccess__) */
