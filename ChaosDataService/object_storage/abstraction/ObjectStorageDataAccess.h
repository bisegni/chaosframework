/*
 *	ObjectStorageDataAccess.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/09/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_C9E19CC7_5691_4873_9DBC_39596C17E8C2_ObjectStorageDataAccess_h
#define __CHAOSFramework_C9E19CC7_5691_4873_9DBC_39596C17E8C2_ObjectStorageDataAccess_h

#include <chaos/common/chaos_types.h>

#include <chaos/common/data/CDataWrapper.h>

#include <chaos_service_common/persistence/data_access/AbstractDataAccess.h>

namespace chaos {
    namespace data_service {
        namespace object_storage {
            namespace abstraction {
                
                typedef boost::shared_ptr<chaos::common::data::CDataWrapper> ObjectSharedPtr;
                
                //!Object vector
                CHAOS_DEFINE_VECTOR_FOR_TYPE(ObjectSharedPtr, VectorObject);
                
                //!CHaos abstraction for store time series data wintin a persistence sublayer
                class ObjectStorageDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                public:
                    DECLARE_DA_NAME
                    //! default constructor
                    ObjectStorageDataAccess();
                    
                    //! defautl destructor
                    ~ObjectStorageDataAccess();
                    
                    //!Put an object within the object persistence layer
                    virtual int pushObject(const std::string& key,
                                           const chaos::common::data::CDataWrapper& stored_object) = 0;
                    
                    //!Retrieve an object from the object persistence layer
                    virtual int getObject(const std::string& key,
                                          const uint64_t& timestamp,
                                          ObjectSharedPtr& object_ptr_ref) = 0;
                    
                    //!delete objects that are contained between intervall (exstreme included)
                    virtual int deleteObject(const std::string& key,
                                             uint64_t start_timestamp,
                                             uint64_t end_timestamp) = 0;
                    
                    //!search object into object persistence layer
                    virtual int findObject(const std::string& key,
                                           const uint64_t timestamp_from,
                                           const uint64_t timestamp_to,
                                           const bool from_is_included,
                                           const int page_len,
                                           VectorObject& found_object_page,
                                           uint64_t& last_daq_timestamp) = 0;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_C9E19CC7_5691_4873_9DBC_39596C17E8C2_ObjectStorageDataAccess_h */
