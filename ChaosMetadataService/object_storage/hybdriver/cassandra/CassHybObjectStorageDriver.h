/*
 * Copyright 2012, 25/05/2018 INFN
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

#ifndef chaos_metadata_service_object_storage_hybdriver_HybCassDriver_h
#define chaos_metadata_service_object_storage_hybdriver_HybCassDriver_h

#include "../HybBaseDriver.h"
#include "HybCassandraDataType.h"

#include <cassandra.h>

namespace chaos {
    namespace metadata_service {
        namespace object_storage {
            namespace hybdriver {
                namespace cassandra {
                    extern chaos::common::utility::ObjectFactoryInstancer<chaos::service_common::persistence::data_access::AbstractPersistenceDriver> HybCassDriverObjectFactoryInstancer;
                    
                    class CassHybObjectStorageDriver:
                    public HybBaseDriver {
                        friend class chaos::common::utility::ObjectFactoryAliasInstantiation<chaos::service_common::persistence::data_access::AbstractPersistenceDriver>;
                        
                        metadata_service::object_storage::hybdriver::cassandra::CassClusterShrdPtr cluster_shrd_ptr;
                        metadata_service::object_storage::hybdriver::cassandra::CassSessionShrdPtr session_shrd_ptr;
                        
                    protected:
                        virtual HybBaseDataAccess *dataAccessImpl();
                    public:
                        CassHybObjectStorageDriver(const std::string& name);
                        ~CassHybObjectStorageDriver();
                        
                        //! Initialize the driver
                        void init(void *init_data) throw (chaos::CException);
                        
                        //!deinitialize the driver
                        void deinit() throw (chaos::CException);
                    };
                }
            }
        }
    }
}

#endif /* chaos_metadata_service_object_storage_hybdriver_HybCassDriver_h */
