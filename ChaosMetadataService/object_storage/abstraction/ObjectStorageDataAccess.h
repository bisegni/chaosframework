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

#ifndef __CHAOSFramework_C9E19CC7_5691_4873_9DBC_39596C17E8C2_ObjectStorageDataAccess_h
#define __CHAOSFramework_C9E19CC7_5691_4873_9DBC_39596C17E8C2_ObjectStorageDataAccess_h

#include <chaos/common/chaos_types.h>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/direct_io/channel/DirectIODeviceChannelGlobal.h>

#include <chaos_service_common/persistence/data_access/AbstractDataAccess.h>

namespace chaos {
    namespace metadata_service {
        namespace object_storage {
            namespace abstraction {
                
                //!Object vector
                CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::common::data::CDWShrdPtr, VectorObject);
                
                //!informtion needed for the data search
                typedef struct DataSearch {
                    std::string key;
                    ChaosStringSet meta_tags;
                    ChaosStringSet projection_keys;
                    uint64_t timestamp_from;
                    uint64_t timestamp_to;
                    uint32_t page_len;
                } DataSearch;
                
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
                                           const ChaosStringSetConstSPtr meta_tags,
                                           const chaos::common::data::CDataWrapper& stored_object) = 0;
                    
                    //!Retrieve an object from the object persistence layer
                    virtual int getObject(const std::string& key,
                                          const uint64_t& timestamp,
                                          chaos::common::data::CDWShrdPtr& object_ptr_ref) = 0;
                    
                    //!Retrieve the last inserted object from the object persistence layer
                    virtual int getLastObject(const std::string& key,
                                              chaos::common::data::CDWShrdPtr& object_ptr_ref) = 0;
                    
                    //!delete objects that are contained between intervall (exstreme included)
                    virtual int deleteObject(const std::string& key,
                                             uint64_t start_timestamp,
                                             uint64_t end_timestamp) = 0;
                    
                    //!search object into object persistence layer
                    virtual int findObject(const std::string& key,
                                           const ChaosStringSet& meta_tags,
                                           const ChaosStringSet& projection_keys,
                                           const uint64_t timestamp_from,
                                           const uint64_t timestamp_to,
                                           const uint32_t page_len,
                                           VectorObject& found_object_page,
                                           chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) = 0;
                    
                    //!fast search object into object persistence layer
                    /*!
                     Fast search return only data index to the client, in this csae client ned to use api to return the single
                     or grouped data
                     */
                    virtual int findObjectIndex(const DataSearch& search,
                                                VectorObject& found_object_page,
                                                chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) = 0;
                    
                    //! return the object asosciated with the index array
                    /*!
                     For every index object witl be returned the associated data object, if no data is received will be
                     insert an empty object
                     */
                    virtual int getObjectByIndex(const chaos::common::data::CDWShrdPtr& index,
                                                 chaos::common::data::CDWShrdPtr& found_object) = 0;
                    
                    //!return the number of object for a determinated key that are store for a time range
                    virtual int countObject(const std::string& key,
                                            const uint64_t timestamp_from,
                                            const uint64_t timestamp_to,
                                            const uint64_t& object_count) = 0;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_C9E19CC7_5691_4873_9DBC_39596C17E8C2_ObjectStorageDataAccess_h */
