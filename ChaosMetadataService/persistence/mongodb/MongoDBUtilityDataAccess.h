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
#ifndef __CHAOSFramework__MongoDBUtilityDataAccess__
#define __CHAOSFramework__MongoDBUtilityDataAccess__

#include "../data_access/UtilityDataAccess.h"

#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for producer manipulation data
                class MongoDBUtilityDataAccess:
                public data_access::UtilityDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                protected:
                    MongoDBUtilityDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBUtilityDataAccess();
                public:
                    //inherited method
                    int getNextSequenceValue(const std::string& sequence_name, uint64_t& next_value);
                    
                    //inherited method
                    int resetAllData();
                    
                    //inherited method
                    int setVariable(const std::string& variable_name,
                                    chaos::common::data::CDataWrapper& cdw);
                    
                    //inherited method
                    int getVariable(const std::string& variable_name,
                                    chaos::common::data::CDataWrapper **cdw);
                    
                    //inherited method
                    int deleteVariable(const std::string& variable_name);
                };
                
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBUtilityDataAccess__) */
