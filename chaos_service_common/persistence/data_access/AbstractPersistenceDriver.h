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
                    void registerDataAccess(S *instance) {
                        const std::string alias = S::name;
                        map_data_access.insert(make_pair(alias, new DAInstanceContainer<S>(instance)));
                    }
                    virtual void deleteDataAccess(void *instance) = 0;
                public:
                    AbstractPersistenceDriver(const std::string& name);
                    virtual ~AbstractPersistenceDriver();
                    
                    //! Initialize the driver
                    void init(void *init_data) ;
                    
                    //!deinitialize the driver
                    void deinit() ;
                    
                    //! return the implementation of the unit server data access
                    template<typename D>
                    D *getDataAccess() {
                        return static_cast<D*>(map_data_access[D::name]->getInstance());
                    }

                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__AbstractMSDatabaseDriver__) */
