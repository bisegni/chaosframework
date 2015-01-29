/*
 *	UnitServerDataAccess.h
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


#ifndef __CHAOSFramework__UnitServerDataAccess__
#define __CHAOSFramework__UnitServerDataAccess__

#include "../AbstractDataAccess.h"

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                class UnitServerDataAccess:
                public AbstractDataAccess {
                public:
                    //! default constructor
                    UnitServerDataAccess();
                    
                    //! defautl destructor
                    ~UnitServerDataAccess();
                    
                    //! insert a new device with name and property
                    virtual int insertNewUnitServer(const std::string& unit_server_name,
                                                    const std::string& unit_server_rpc_url,
                                                    std::vector<std::string> hosted_cu_class) = 0;
                    
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__UnitServerDataAccess__) */
