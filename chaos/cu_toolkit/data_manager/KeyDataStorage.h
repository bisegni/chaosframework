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

#ifndef KeyDataStorage_H
#define KeyDataStorage_H

#include <map>
#include <string>

#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/utility/ArrayPointer.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

namespace chaos_data = chaos::common::data;
namespace chaos_io = chaos::common::io;
namespace chaos_utility = chaos::common::utility;
namespace chaos{
    namespace cu {
        namespace data_manager {
            
            typedef enum KeyDataStorageDomain {
                KeyDataStorageDomainOutput = DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT,
                KeyDataStorageDomainInput = DataPackCommonKey::DPCK_DATASET_TYPE_INPUT,
                KeyDataStorageDomainCustom = DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM,
                KeyDataStorageDomainSystem =  DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM,
                KeyDataStorageDomainHealth = DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH,
                KeyDataStorageDomainDevAlarm =DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM,
                KeyDataStorageDomainCUAlarm =DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM
            } KeyDataStorageDomain;
            
            class KeyDataStorage {
                std::string key;
                std::string output_key;
                std::string input_key;
                std::string system_key;
                std::string custom_key;
                std::string health_key;
                std::string cu_alarm_key;
                std::string dev_alarm_key;
                //is the sequence if
                boost::atomic<int64_t> sequence_id;
                
                //restore poitn map
                std::map<std::string, std::map<std::string, ChaosSharedPtr<chaos_data::CDataWrapper> > > restore_point_map;
                
                chaos_io::IODataDriver *io_data_driver;
                
                //storage type
                DataServiceNodeDefinitionType::DSStorageType storage_type;
                
                //history time
                uint64_t storage_history_time;
                uint64_t storage_history_time_last_push;
                uint64_t storage_live_time;
                uint64_t storage_live_time_last_push;
                //mutex to protect access to data io driver
                boost::mutex mutex_push_data;
                
                void pushDataWithControlOnHistoryTime(const std::string& key,
                                                      chaos::common::data::CDataWrapper *dataToStore,
                                                      chaos::DataServiceNodeDefinitionType::DSStorageType storage_type);
                
                inline std::string getDomainString(const KeyDataStorageDomain dataset_domain);
                
                inline std::string getDomainString(const std::string& node_uid,
                                                   const KeyDataStorageDomain dataset_domain);
            public:
                KeyDataStorage(const std::string& _key,
                               chaos_io::IODataDriver *_io_data_driver);
                virtual ~KeyDataStorage();
                
                void init(void *init_parameter) throw (chaos::CException);
                
                void deinit() throw (chaos::CException);
                /*
                 Return a new instace for the CSDatawrapped
                 */
                chaos_data::CDataWrapper* getNewDataPackForDomain(const KeyDataStorageDomain domain);
                
                /*
                 Retrive the data from Live Storage
                 */
                //chaos_utility::ArrayPointer<chaos_data::CDataWrapper>* getLastDataSet(KeyDataStorageDomain domain);
                
                //! push a dataset associated to a domain
                void pushDataSet(KeyDataStorageDomain domain, chaos_data::CDataWrapper *dataset);
                
                //! load all dataseet for a restore point
                int loadRestorePoint(const std::string& restore_point_tag);
                
                //! clear all loaded dataset for a restore point
                int clearRestorePoint(const std::string& restore_point_tag);
                
                //!return the dataset asccoaited to a domain in a determinated restore tag
                ChaosSharedPtr<chaos_data::CDataWrapper>  getDatasetFromRestorePoint(const std::string& restore_point_tag,
                                                                                     KeyDataStorageDomain domain);
                
                /*
                 Permit to be live configurable
                 */
                void updateConfiguration(const std::string& conf_name, const chaos::common::data::CDataVariant& conf_value);
                
                //!return the current storage type [live, history or both] setting
                DataServiceNodeDefinitionType::DSStorageType getStorageType();
                
                //! return how how many mseconds need to pass for the dataset is stored in live cache
                uint64_t getStorageLiveTime();
                
                //! return how how many mseconds need to pass for the dataset is stored in history engine
                uint64_t getStorageHistoryTime();
                
                //! perform live search for managed node uid
                int performLiveFetch(const KeyDataStorageDomain dataset_domain,
                                     chaos::common::data::CDWShrdPtr& found_dataset);
                
                //! perform live search for other node uid
                int performLiveFetch(const ChaosStringVector& node_uid,
                                     const KeyDataStorageDomain dataset_domain,
                                     chaos::common::data::VectorCDWShrdPtr& found_dataset);
                
                int performSelfQuery(chaos::common::io::QueryCursor **cursor,
                                     KeyDataStorageDomain dataset_domain,
                                     const uint64_t start_ts,
                                     const uint64_t end_ts);
                
                int performGeneralQuery(chaos::common::io::QueryCursor **cursor,
                                        const std::string& node_id,
                                        KeyDataStorageDomain dataset_domain,
                                        const uint64_t start_ts,
                                        const uint64_t end_ts);
                
                void releseQuery(chaos::common::io::QueryCursor *cursor);
            };
        }
    }
}
#endif
