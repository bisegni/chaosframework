/*
 *	AbstractDataAccess.h
 *	!CHAOS
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
#ifndef __CHAOSFramework__AbstractDataAccess__
#define __CHAOSFramework__AbstractDataAccess__

#include <chaos/common/utility/NamedService.h>

#define DECLARE_DA_NAME static const char * const name;
#define DEFINE_DA_NAME(x) const char * const x::name=#x;

namespace chaos {
    namespace service_common {
        namespace persistence {
            namespace data_access {
                //! Data Access base class
                /*!
                 The data access are those class the directly work on data.
                 Every implementation will perform the data operation
                 on different persistence engine. The sublclass of first level need to define
                 the api on the data and the second level subclass needs to define the implementation
                 */
                class AbstractDataAccess:
                public chaos::common::utility::NamedService {

                public:
                    AbstractDataAccess(const std::string& name);
                    virtual ~AbstractDataAccess();
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__AbstractDataAccess__) */
