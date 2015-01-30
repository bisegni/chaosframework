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

#include "MongoDBAccessor.h"
#include "../data_access/ProducerDataAccess.h"

#include <chaos/common/utility/ObjectINstancer.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
	namespace metadata_service {
		namespace persistence {
			namespace mongodb {
				
				//forward declaration
				class MongoDBPersistenceDriver;
				
                //! Data Access for producer manipulation data
				class MongoDBProducerDataAccess:
                protected MongoDBAccessor,
				protected data_access::ProducerDataAccess {
					friend class INSTANCER_P1(MongoDBProducerDataAccess, AbstractDataAccess, const boost::shared_ptr<MongoDBHAConnectionManager>&);

				protected:
                    MongoDBProducerDataAccess(const boost::shared_ptr<MongoDBHAConnectionManager>& _connection);
					~MongoDBProducerDataAccess();
				public:
                    //! insert a new device with name and property
                    virtual int insertNewProducer(const std::string& producer_unique_name,
                                                  chaos::common::data::CDataWrapper& producer_property);
                    
                    //! check if a device is registered on persistence layer
                    virtual int checkProducerPresence(const std::string& producer_unique_name, bool& found);
                    
                    //! save the new dataset of a producer
                    virtual int saveDataset(const std::string& producer_unique_name,
                                            chaos::common::data::CDataWrapper& dataset_to_register);
                    
                    //! return last dataaset of a producer
                    virtual int loadLastDataset(const std::string& producer_unique_name,
                                                chaos::common::data::CDataWrapper **dataset_to_load);
				};
			}
		}
	}	
}
#endif /* defined(__CHAOSFramework__MongoDBProducerDataAccess__) */
