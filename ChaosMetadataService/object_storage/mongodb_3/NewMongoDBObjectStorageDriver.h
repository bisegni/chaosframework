/*
 * Copyright 2012, 18/06/2018 INFN
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

#ifndef __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_NewMongoDBObjectStorageDriver_h
#define __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_NewMongoDBObjectStorageDriver_h

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos_service_common/persistence/mongodb/mongodb_cxx/MongoDBCXXDriver.h>
#include <chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.h>
#include <chaos_service_common/persistence/data_access/AbstractPersistenceDriver.h>

namespace chaos {
    namespace metadata_service {
        namespace object_storage {
            namespace mongodb_3 {
                
                //! new mongodb implementation of persistence driver
                /*!
                 The driver is define as class in the object factor
                 */
                DECLARE_CLASS_FACTORY(NewMongoDBObjectStorageDriver,
                                      chaos::service_common::persistence::data_access::AbstractPersistenceDriver),
                public chaos::service_common::persistence::mongodb::mongodb_cxx::BaseMongoDBDiver {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(NewMongoDBObjectStorageDriver)
                    
                    //! Construct the driver
                    NewMongoDBObjectStorageDriver(const std::string& name);
                    
                    //!dispose the driver
                    ~NewMongoDBObjectStorageDriver();
                    
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

#endif /* __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_NewMongoDBObjectStorageDriver_h */
