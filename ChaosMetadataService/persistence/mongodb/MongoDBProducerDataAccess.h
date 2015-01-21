/*
 *	MongoDBProducerDataAccess.h
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
#ifndef __CHAOSFramework__MongoDBProducerDataAccess__
#define __CHAOSFramework__MongoDBProducerDataAccess__

#include "MongoDBHAConnectionManager.h"
#include "../data_access/ProducerDataAccess.h"

#include <boost/shared_ptr.hpp>

namespace chaos {
	namespace metadata_service {
		namespace persistence {
			namespace mongodb {
				
				//forward declaration
				class MongoDBPersistenceDriver;
				
				class MongoDBProducerDataAccess:
				protected data_access::ProducerDataAccess {
					friend class MongoDBPersistenceDriver;
					
					boost::shared_ptr<MongoDBHAConnectionManager> mongodb_connection;
					
				protected:
					MongoDBProducerDataAccess(boost::shared_ptr<MongoDBHAConnectionManager>& _mongodb_connection);
					~MongoDBProducerDataAccess();
				public:
					int saveDataset(chaos::common::data::CDataWrapper& dataset_to_register);
					
					
					int loadLastDataset(const std::string& producer_name,
										chaos::common::data::CDataWrapper **dataset_to_register);
				};
			}
		}
	}	
}
#endif /* defined(__CHAOSFramework__MongoDBProducerDataAccess__) */
