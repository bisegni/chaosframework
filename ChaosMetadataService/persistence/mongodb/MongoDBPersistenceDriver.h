/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#ifndef __CHAOSFramework__MongoDBPersistenceDriver__
#define __CHAOSFramework__MongoDBPersistenceDriver__

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.h>
#include <chaos_service_common/persistence/data_access/AbstractPersistenceDriver.h>

namespace chaos {
	namespace metadata_service {
		namespace persistence {
			namespace mongodb {
					

#define DB_NAME "db"
                
				//! mongodb implementation of persistence driver
                /*!
                 The driver is define as class in the object factor
                 */
                DECLARE_CLASS_FACTORY(MongoDBPersistenceDriver,
                                      chaos::service_common::persistence::data_access::AbstractPersistenceDriver) {
					REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(MongoDBPersistenceDriver)
					
                    //!keep track of the allocated connection
                    ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager> connection;

                    //! Construct the driver
					MongoDBPersistenceDriver(const std::string& name);
                    
                    //!dispose the driver
					~MongoDBPersistenceDriver();
					
                    //!inherited by AbstractPersistenceDriver
                    void deleteDataAccess(void *instance);
				public:
                    //! Initialize the driver
					void init(void *init_data) ;
                    
                    //!deinitialize the driver
					void deinit() ;
				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__MongoDBPersistenceDriver__) */
