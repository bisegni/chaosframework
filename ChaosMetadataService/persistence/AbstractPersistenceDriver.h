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

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service{
        namespace persistence {
            
            //! namespace for data access type collection
            namespace data_access_type {
                typedef enum DataAccessType {
                    DataAccessTypeProducer = 0,
                    DataAccessTypeUnitServer
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
                
                //! map for the dataaccess allocation
                std::map<data_access_type::DataAccessType, void* > map_dataaccess_instances;
                
                //! muthex for DA instances access
                boost::shared_mutex map_mutex;
            protected:
                
                //!register a dataaccess implementation with base name ampping
                template<typename T, typename p1>
                void registerDataAccess(data_access_type::DataAccessType da_type,
                                        p1 param_1) {
                    //!che if dataaccess already present
                    if(map_dataaccess_instances.count(da_type)) return;
                    
                    //allocate the instancer for the AbstractApi depending by the template
                    std::auto_ptr<INSTANCER_P1(T, AbstractDataAccess, p1)> i(ALLOCATE_INSTANCER_P1(T, AbstractDataAccess, p1));
                    
                    //get api instance
                    T *instance = (T*)i->getInstance(param_1);
                    if(instance) {
                        map_dataaccess_instances.insert(make_pair(da_type, instance));
                    }
                    
                }
                
            public:
                AbstractPersistenceDriver(const std::string& name);
                virtual ~AbstractPersistenceDriver();
                
                //! return the implementation of the producer data access
                virtual data_access::ProducerDataAccess *getProducerDataAccess();
                
                //! return the implementation of the unit server data access
                virtual data_access::UnitServerDataAccess *getUnitServerDataAccess();
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__AbstractMSDatabaseDriver__) */
