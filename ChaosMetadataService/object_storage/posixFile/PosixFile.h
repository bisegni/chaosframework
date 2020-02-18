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

#ifndef __CHAOSFramework_C9E19CC7_5691_4873_9DBC_39596C17E8C2_PosixFile_h
#define __CHAOSFramework_C9E19CC7_5691_4873_9DBC_39596C17E8C2_PosixFile_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/async_central/async_central.h>
#include <boost/iostreams/device/mapped_file.hpp>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/direct_io/channel/DirectIODeviceChannelGlobal.h>

#include "../abstraction/ObjectStorageDataAccess.h"
#if CHAOS_PROMETHEUS
#include <chaos/common/metric/metric.h>
#endif //CHAOS_PROMETHEUS
// 3Khz
#define MAX_NUM_OF_FILE_PER_MINUTE 60*3000
namespace chaos {
    namespace metadata_service {
        namespace object_storage {
           class PosixStorageDriver;
                
            class PosixFile:public metadata_service::object_storage::abstraction::ObjectStorageDataAccess,public chaos::common::async_central::TimerHandler {



                    std::string basedatapath;
                    static std::string serverName;
                    friend class PosixStorageDriver;
                    int removeRecursevelyUp(const boost::filesystem::path& p );
                    void calcFileDir(const std::string& prefix, const std::string&tag,const std::string& cu, uint64_t ts_ms, uint64_t seq, uint64_t runid, char* dir, char* fname);
                    uint32_t countFromPath(boost::filesystem::path& p,const uint64_t timestamp_from,
                          const uint64_t timestamp_to);

                    int getFromPath(const std::string& p,const uint64_t timestamp_from,
                          const uint64_t timestamp_to,
                          const uint32_t page_len,
                          abstraction::VectorObject& found_object_page,
                          chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq);

#if CHAOS_PROMETHEUS
    chaos::common::metric::CounterUniquePtr counter_write_data_uptr;
    chaos::common::metric::CounterUniquePtr counter_read_data_uptr;
    chaos::common::metric::GaugeUniquePtr gauge_insert_time_uptr;
    chaos::common::metric::GaugeUniquePtr gauge_query_time_uptr;
#endif
public:
                typedef struct {ChaosSharedMutex devio_mutex;std::vector<std::string> keys;uint64_t ts;boost::iostreams::mapped_file mf;std::string fname;uint32_t size;uint32_t max_size; uint32_t nobj;uint64_t last_seq;uint64_t last_runid;  bson_writer_t *writer;
} fdw_t;
                typedef struct {ChaosSharedMutex devio_mutex;uint64_t ts;uint64_t obj;} ordered_info_t;
                typedef std::map<std::string,fdw_t> write_path_t;
                typedef std::map<std::string,ordered_info_t> ordered_t;
                static ordered_t s_ordered;
                static  write_path_t s_lastWriteDir;
                typedef struct __{ std::vector<std::string> sorted_path;uint64_t ts;ChaosSharedMutex devio_mutex;} read_path_t;
                typedef std::map<std::string,read_path_t> cacheRead_t; 
                static ChaosSharedMutex last_access_mutex;
                static cacheRead_t s_lastAccessedDir;
                // return number of items, or negative if error
                int reorderData(const std::string& dstpath,const std::vector<std::string>&ordered_keys,int size,const std::string& srcpath);

                void timeout();

                public:
                      //! Construct the driver
                    PosixFile(const std::string& name);
                    //! defautl destructor
                    ~PosixFile();
                       //!dispose the driver                   
                    //!Put an object within the object persistence layer
                    virtual int pushObject(const std::string& key,
                                           const ChaosStringSetConstSPtr meta_tags,
                                           const chaos::common::data::CDataWrapper& stored_object) ;
                    
                    //!Retrieve an object from the object persistence layer
                    virtual int getObject(const std::string& key,
                                          const uint64_t& timestamp,
                                          chaos::common::data::CDWShrdPtr& object_ptr_ref);
                    
                    //!Retrieve the last inserted object from the object persistence layer
                    virtual int getLastObject(const std::string& key,
                                              chaos::common::data::CDWShrdPtr& object_ptr_ref);
                    
                    //!delete objects that are contained between intervall (exstreme included)
                    virtual int deleteObject(const std::string& key,
                                             uint64_t start_timestamp,
                                             uint64_t end_timestamp);
                    
                    //!search object into object persistence layer
                    virtual int findObject(const std::string& key,
                                           const ChaosStringSet& meta_tags,
                                           const ChaosStringSet& projection_keys,
                                           const uint64_t timestamp_from,
                                           const uint64_t timestamp_to,
                                           const uint32_t page_len,
                                           abstraction::VectorObject& found_object_page,
                                           chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq);
                    
                    //!fast search object into object persistence layer
                    /*!
                     Fast search return only data index to the client, in this csae client ned to use api to return the single
                     or grouped data
                     */
                    virtual int findObjectIndex(const abstraction::DataSearch& search,
                                                abstraction::VectorObject& found_object_page,
                                                chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq);
                    
                    //! return the object asosciated with the index array
                    /*!
                     For every index object witl be returned the associated data object, if no data is received will be
                     insert an empty object
                     */
                    virtual int getObjectByIndex(const chaos::common::data::CDWShrdPtr& index,
                                                 chaos::common::data::CDWShrdPtr& found_object);
                    
                    //!return the number of object for a determinated key that are store for a time range
                    virtual int countObject(const std::string& key,
                                            const uint64_t timestamp_from,
                                            const uint64_t timestamp_to,
                                             uint64_t& object_count);
                };
            }
        }
    
}

#endif /* __CHAOSFramework_C9E19CC7_5691_4873_9DBC_39596C17E8C2_PosixFile_h */
