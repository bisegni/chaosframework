/*
 *	AbstractPersistenceDriver.h
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
#ifndef __CHAOSFramework__AbstractMSDatabaseDriver__
#define __CHAOSFramework__AbstractMSDatabaseDriver__

#include "AbstractDataAccess.h"
#include "data_access/DataAccess.h"

#include <map>
#include <string>

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/InizializableService.h>

namespace chaos {
    namespace metadata_service{
        namespace persistence {
            
            //! namespace for data access type collection
            namespace data_access_type {
                typedef enum DataAccessType {
                    DataAccessTypeProducer = 0,
                    DataAccessTypeUnitServer = 1
                } DataAccessType;
            }
            
            //! Abstract base persistence driver
            /*!
             Define the rule for the persistence sublcass implementation.
             The scope of this class i instantiate the implementation of
             the DataAccess base class
             */
            class AbstractPersistenceDriver:
            public common::utility::NamedService,
            public common::utility::InizializableService {
                
                //!data access instances
                AbstractDataAccess *producer_da_instance;
                AbstractDataAccess *unit_server_da_instance;
            protected:
                
                //!register a dataaccess implementation with base name ampping
                template<typename T, typename p1>
                void registerDataAccess(data_access_type::DataAccessType da_type,
                                        p1 param_1) {
                    //allocate the instancer for the AbstractApi depending by the template
                    std::auto_ptr<INSTANCER_P1(T, AbstractDataAccess, p1)> i(ALLOCATE_INSTANCER_P1(T, AbstractDataAccess, p1));
                    
                    //get api instance
                    T *instance = (T*)i->getInstance(param_1);
                    if(instance) {
                        switch(da_type) {
                            case data_access_type::DataAccessTypeProducer:
                                producer_da_instance = instance;
                                break;
                                
                            case data_access_type::DataAccessTypeUnitServer:
                                unit_server_da_instance = instance;
                                break;
                            
                            default:
                                break;
                        }
                    }
                    
                }
                
            public:
                AbstractPersistenceDriver(const std::string& name);
                virtual ~AbstractPersistenceDriver();
                
                //! Initialize the driver
                void init(void *init_data) throw (chaos::CException);
                
                //!deinitialize the driver
                void deinit() throw (chaos::CException);
                
                //! return the implementation of the producer data access
                data_access::ProducerDataAccess *getProducerDataAccess();
                
                //! return the implementation of the unit server data access
                data_access::UnitServerDataAccess *getUnitServerDataAccess();

            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__AbstractMSDatabaseDriver__) */
