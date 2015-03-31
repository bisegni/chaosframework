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

#include <chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.h>

#include <boost/format.hpp>


    //! add a regex on a field
/*!
 create a return a BSON object with a query expresion with a regular expression like:
 { field_name: {$regex:regex}}
 */
#define MONGODB_REGEX_ON_FILED(field_name, regex) BSON(field_name << BSON("$regex" << regex))

namespace chaos {
    namespace service_common {
        namespace persistence {
            namespace mongodb {

                typedef std::vector<mongo::BSONObj>             SearchResult;
                typedef std::vector<mongo::BSONObj>::iterator   SearchResultIterator;

                class MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                protected:
                    boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager> connection;
                    
                public:
                    MongoDBAccessor(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    virtual ~MongoDBAccessor();
                    
                    const std::string& getDatabaseName();
                    
                    //! perform a paged query
                    /*!
                     perform a query using paging logic
                     \param paged_result a vectorot that will be filled with all found data
                     \param q is the query that perform tath filter the result
                     \param prj is the set of field that we whant to be returned
                     \param from is the last value that we have found on previous searh
                     \param limit is the number of the element that we need to retrieve
                     */
                    int performPagedQuery(SearchResult& paged_result,
                                          const std::string& db_collection,
                                          mongo::Query q,
                                          mongo::BSONObj *prj = NULL,
                                          mongo::BSONObj *from = NULL,
                                          uint32_t limit = 50);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBAccessor__) */
