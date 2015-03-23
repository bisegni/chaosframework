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

#include <chaos_service_common/persistence/data_access/AbstractDataAccess.h>

#include <map>
#include <string>

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/InizializableService.h>

namespace chaos {
    namespace service_common {
        namespace persistence {
            namespace data_access {
                
                
                struct InstanceContainer {
                    virtual void* getInstance() = 0;
                    InstanceContainer(){};
                    virtual ~InstanceContainer(){};
                };
                
                template<typename S>
                struct DAInstanceContainer:
                public InstanceContainer {
                    S *instance;
                    DAInstanceContainer(S *_instance):instance(_instance){};
                    ~DAInstanceContainer(){if(instance) delete(instance);}
                    
                    void* getInstance(){
                        return static_cast<void*>(instance);
                    }
                };
                
                typedef std::map<std::string, InstanceContainer*>              MapDA;
                typedef std::map<std::string, InstanceContainer*>::iterator    MapDAIterator;
                
                //! Abstract base persistence driver
                /*!
                 Define the rule for the persistence sublcass implementation.
                 The scope of this class i instantiate the implementation of
                 the DataAccess base class
                 */
                class AbstractPersistenceDriver:
                public common::utility::NamedService,
                public common::utility::InizializableService {
                    
                    MapDA map_data_access;
                    
                protected:
                    
                    //!register a dataaccess implementation with base name ampping
                    template<typename S>
                    void registerDataAccess(const std::string& alias, S *instance) {
                        map_data_access.insert(make_pair(alias, new DAInstanceContainer<S>(instance)));
                    }
                    virtual void deleteDataAccess(void *instance) = 0;
                public:
                    AbstractPersistenceDriver(const std::string& name);
                    virtual ~AbstractPersistenceDriver();
                    
                    //! Initialize the driver
                    void init(void *init_data) throw (chaos::CException);
                    
                    //!deinitialize the driver
                    void deinit() throw (chaos::CException);
                    
                    //! return the implementation of the unit server data access
                    template<typename D>
                    D *getDataAccess(const std::string& alias) {
                        return static_cast<D*>(map_data_access[alias]->getInstance());
                    }

                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__AbstractMSDatabaseDriver__) */
