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

#ifndef __CHAOSFramework__MongoDBAccessor__
#define __CHAOSFramework__MongoDBAccessor__

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/persistence/mongodb/MongoDBHAConnectionManager.h>

#include <boost/format.hpp>


//#define MONGO_DB_COLLECTION_NAME(db,coll)	boost::str(boost::format("%1%.%2%") % db % coll)
#undef MONGO_DB_COLLECTION_NAME
#define MONGO_DB_COLLECTION_NAME(coll)	boost::str(boost::format("%1%.%2%") % getDatabaseName() % coll)
//! add a regex on a field
/*!
 create a return a BSON object with a query expresion with a regular expression like:
 { field_name: {$regex:regex}}
 */
#define MONGODB_REGEX_ON_FILED(field_name, regex) BSON(field_name << BSON("$regex" << regex))

//copy chaos data wrapper key into the bson obj builder
#define MDB_COPY_STRING_CDWKEY_TO_BUILDER(b, o, k)\
if(o->hasKey(k)) {\
b << k << o->getStringValue(k);\
}

#define MDB_COPY_I32_CDWKEY_TO_BUILDER(b, o, k)\
if(o->hasKey(k)) {\
b << k << o->getInt32Value(k);\
}

namespace chaos {
    namespace service_common {
        namespace persistence {
            namespace mongodb {
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(mongo::BSONObj, SearchResult)
                
                class MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                protected:
                    ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager> connection;
                    
                public:
                    MongoDBAccessor(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    virtual ~MongoDBAccessor();
                    
                    const std::string& getDatabaseName();
                    
                    mongo::BSONObj regexOnField(const std::string& field_name,
                                                const std::string& regex);
                    
                    //! search within an array with a list of simple query()key/value
                    /*!
                     \param search_keys_values the key valu epairs that are the simple mongodb query(key/value)
                     */
                    mongo::BSONObj arrayMatch(const std::vector< std::pair<std::string, std::string> >& search_keys_values);
                    
                    //! search within an array with simple query key/value
                    /*!
                     \param serach_key the key for the field
                     \param search_value the value corresponding to the key
                     */
                    mongo::BSONObj arrayMatch(const std::string& serach_key,
                                              const std::string& search_value);
                    
                    //! create the array of 'or' condition with all token in search string
                    /*!
                     create the 'or' query for all token in search string for search all they in
                     a field of a collection
                     \param search_string the set of token to search witin a collection field
                     \param field_target_for_search the field target of the search
                     */
                    mongo::BSONArray getSearchTokenOnFiled(const std::string& search_string,
                                                           const std::string& field_target_for_search);
                    
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
