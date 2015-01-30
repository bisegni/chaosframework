/*
 *	MongoDBAccessor.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__MongoDBAccessor__
#define __CHAOSFramework__MongoDBAccessor__

#include "MongoDBHAConnectionManager.h"

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                
                class MongoDBPersistenceDriver;
                
                class MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    std::string database_name;
                    
                    boost::shared_ptr<MongoDBHAConnectionManager> connection;
                protected:

                    
                public:
                    MongoDBAccessor(const boost::shared_ptr<MongoDBHAConnectionManager>& _connection);
                    virtual ~MongoDBAccessor();
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBAccessor__) */
