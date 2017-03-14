/*
 *	IODataDriver.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef IODataDriver_H
#define IODataDriver_H
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <chaos/common/exception/exception.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/utility/ArrayPointer.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/io/QueryCursor.h>

namespace chaos_data = chaos::common::data;

namespace chaos{
    namespace common {
        namespace io {
            
            /*!
             * History Output driver base abstract classe, that define two method to control
             * the initialization and history persistence of the data
             */
            class IODataDriver:
            public Configurable,
            public chaos::common::utility::InizializableService {
            protected:
                QueryCursor *_getNewQueryFutureForQueryID(const std::string& query_id,
                                                          const std::string& key,
                                                          uint64_t start_ts,
                                                          uint64_t end_ts);
                
                void _releaseQueryFuture(QueryCursor *query_future_ptr);
            public:
                virtual ~IODataDriver(){};
                /*!
                 * Init method, the has map has all received value for configuration
                 * every implemented driver need to get all needed configuration param
                 */
                void init(void *init_parameter) throw(CException);
                /*!
                 * DeInit method
                 */
                void deinit() throw(CException);
                
                /*!
                 * This method cache all object passed to driver
                 * \param storage_type one of values as @DataServiceNodeDefinitionType::DSStorageType
                 */
                void storeData(const std::string& key,
                               chaos_data::CDataWrapper *dataToStore,
                               DataServiceNodeDefinitionType::DSStorageType storage_type,
                               bool delete_data_to_store = true) throw(CException);
                
                virtual void storeHealthData(const std::string& key,
                                             chaos_data::CDataWrapper& dataToStore,
                                             DataServiceNodeDefinitionType::DSStorageType storage_type) throw(CException) = 0;
                
                //!remove data between the time intervall (extreme included) operation is not undoable
                virtual int removeData(const std::string& key,
                                       uint64_t start_ts,
                                       uint64_t end_ts) throw(CException);
                
                /*!
                 * This method retrive the cached object by CSDawrapperUsed as query key and
                 * return a pointer to the class ArrayPointer of CDataWrapper type
                 */
                virtual utility::ArrayPointer<chaos_data::CDataWrapper>* retriveData(const std::string& key,
                                                                                     chaos_data::CDataWrapper*const)  throw(CException);
                
                /*!
                 * This method retrive the cached object by CSDawrapperUsed as query key and
                 * return a pointer to the class ArrayPointer of CDataWrapper type
                 */
                virtual utility::ArrayPointer<chaos_data::CDataWrapper>* retriveData(const std::string& key)  throw(CException);
                
                
                /*!
                 * This method store a buffer into live cached
                 */
                virtual void storeRawData(const std::string& key,
                                          chaos_data::SerializationBuffer *serialization,
                                          DataServiceNodeDefinitionType::DSStorageType storage_type)  throw(CException) = 0;
                
                /*!
                 * This method retrive the cached object by CSDawrapperUsed as query key and
                 * return a pointer to the class ArrayPointer of CDataWrapper type
                 */
                virtual char * retriveRawData(const std::string& key,
                                              size_t* dataDim=NULL)  throw(CException) = 0;
                
                
                //! restore from a tag a dataset associated to a key
                /*!
                 try to load a dataset from snapshot identified by the tag
                 \param snapshot_tag_name the name of the tag that identify the snapshot
                 \param key is the unique key of the producer
                 \param dataset_type is the type of dataset to fetch
                 \param cdatawrapper_handler handler for the found dataset(the deallocation need to be managed by caller)
                 */
                virtual int loadDatasetTypeFromSnapshotTag(const std::string& snapshot_tag_name,
                                                           const std::string& key,
                                                           uint32_t dataset_type,
                                                           chaos_data::CDataWrapper **cdatawrapper_handler) = 0;
                /*!
                 Update the driver configuration
                 */
                virtual chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*);
                
                
                //! perform a query since and
                virtual QueryCursor *performQuery(const std::string& key,
                                                  uint64_t start_ts,
                                                  uint64_t end_ts,uint32_t page_len=DEFAULT_PAGE_LEN) = 0;
                
                virtual void releaseQuery(QueryCursor *query) = 0;
            };
            
        }
    }
}
#endif
