/*
 *	MongoDBObjectStorageDriver.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/09/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_E927A5B7_1CA0_402F_AA20_DD63646EA30A_MongoDBObjectStorageDriver_h
#define __CHAOSFramework_E927A5B7_1CA0_402F_AA20_DD63646EA30A_MongoDBObjectStorageDriver_h

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.h>
#include <chaos_service_common/persistence/data_access/AbstractPersistenceDriver.h>

namespace chaos {
    namespace data_service {
        namespace object_storage {
            namespace mongodb {

#define DB_NAME "db"
                
                //! mongodb implementation of persistence driver
                /*!
                 The driver is define as class in the object factor
                 */
                DECLARE_CLASS_FACTORY(MongoDBObjectStorageDriver,
                                      chaos::service_common::persistence::data_access::AbstractPersistenceDriver) {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(MongoDBObjectStorageDriver)
                    
                    //!keep track of the allocated connection
                    ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager> connection;
                    
                    //! Construct the driver
                    MongoDBObjectStorageDriver(const std::string& name);
                    
                    //!dispose the driver
                    ~MongoDBObjectStorageDriver();
                    
                    //!inherited by AbstractPersistenceDriver
                    void deleteDataAccess(void *instance);
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



#endif /* __CHAOSFramework_E927A5B7_1CA0_402F_AA20_DD63646EA30A_MongoDBObjectStorageDriver_h */
