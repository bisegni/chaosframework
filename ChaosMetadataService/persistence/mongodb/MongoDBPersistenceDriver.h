/*
 *	MongoDBPersistenceDriver.h
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
#ifndef __CHAOSFramework__MongoDBPersistenceDriver__
#define __CHAOSFramework__MongoDBPersistenceDriver__

#include "../AbstractPersistenceDriver.h"
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.h>

namespace chaos {
	namespace metadata_service {
		namespace persistence {
			namespace mongodb {
					

#define DB_NAME "db"
                
				//! mongodb implementation of persistence driver
                /*!
                 The driver is define as class in the object factor
                 */
				DECLARE_CLASS_FACTORY(MongoDBPersistenceDriver, AbstractPersistenceDriver) {
					REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(MongoDBPersistenceDriver)
					
                    //!keep track of the allocated connection
                    boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager> connection;

                    //! Construct the driver
					MongoDBPersistenceDriver(const std::string& name);
                    
                    //!dispose the driver
					~MongoDBPersistenceDriver();
					
				public:
                    //! Initialize the driver
					void init(void *init_data) throw (chaos::CException);
                    
                    //!deinitialize the driver
					void deinit() throw (chaos::CException);
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__MongoDBPersistenceDriver__) */
