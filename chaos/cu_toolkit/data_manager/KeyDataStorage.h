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

#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/data/structured/Dataset.h>
#include <chaos/common/utility/ArrayPointer.h>
#include <chaos/common/utility/LockableObject.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

namespace chaos_data = chaos::common::data;
namespace chaos_io = chaos::common::io;
namespace chaos_utility = chaos::common::utility;
namespace chaos{
    namespace cu {
        namespace data_manager {
            //!data domain managed by the driver
            typedef enum KeyDataStorageDomain {
                KeyDataStorageDomainOutput = DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT,
                KeyDataStorageDomainInput = DataPackCommonKey::DPCK_DATASET_TYPE_INPUT,
                KeyDataStorageDomainCustom = DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM,
                KeyDataStorageDomainSystem =  DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM,
                KeyDataStorageDomainHealth = DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH,
                KeyDataStorageDomainDevAlarm =DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM,
                KeyDataStorageDomainCUAlarm =DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM
            } KeyDataStorageDomain;
            
            //!define tags set
             CHAOS_DEFINE_LOCKABLE_OBJECT(ChaosStringSet, LChaosStringSet);
            
            //!High level driver for manage the push and query of data sets
            class KeyDataStorage {
                const std::string key;
                const std::string output_key;
                const std::string input_key;
                const std::string system_key;
                const std::string custom_key;
                const std::string health_key;
                const std::string cu_alarm_key;
                const std::string dev_alarm_key;
                
                //!is the sequence if
                boost::atomic<int64_t> sequence_id;
                
                //!restore poitn map
                std::map<std::string, std::map<std::string, chaos_data::CDWShrdPtr > > restore_point_map;
                
                //!instance driver for push data
                ChaosUniquePtr<chaos_io::IODataDriver> io_data_driver;
                
                //!storage type
                DataServiceNodeDefinitionType::DSStorageType storage_type;
                
                //!storage type set programmatically
                DataServiceNodeDefinitionType::DSStorageType override_storage_everride;
                
                //!define the queur for burst information
                LChaosStringSet current_tags;
                
                //history time
                uint64_t storage_history_time;
                uint64_t storage_history_time_last_push;
                uint64_t storage_live_time;
                uint64_t storage_live_time_last_push;
                //when tru the timing information set will be used
                bool use_timing_info;
                //mutex to protect access to data io driver
                boost::mutex mutex_push_data;
                // \return 0 if success
                int pushDataWithControlOnHistoryTime(const std::string& key,
                                                      chaos::common::data::CDWShrdPtr dataset,
                                                      chaos::DataServiceNodeDefinitionType::DSStorageType storage_type);
                
                inline std::string getDomainString(const KeyDataStorageDomain dataset_domain);
                
                inline std::string getDomainString(const std::string& node_uid,
                                                   const KeyDataStorageDomain dataset_domain);
            public:
                KeyDataStorage(const std::string& _key,
                               chaos_io::IODataDriver *_io_data_driver);
                virtual ~KeyDataStorage();
                
                void init(void *init_parameter);
                
                void deinit();
                /*
                 Return a new instace for the CSDatawrapped
                 */
                chaos::common::data::CDWShrdPtr getNewDataPackForDomain(const KeyDataStorageDomain domain);
                
                //! push a dataset associated to a domain
                // \return 0 if success
                int pushDataSet(KeyDataStorageDomain domain, chaos::common::data::CDWShrdPtr dataset);
                
                //! load all dataseet for a restore point
                int loadRestorePoint(const std::string& restore_point_tag);
                
                //! clear all loaded dataset for a restore point
                int clearRestorePoint(const std::string& restore_point_tag);
                
                //!return the dataset asccoaited to a domain in a determinated restore tag
                ChaosSharedPtr<chaos_data::CDataWrapper>  getDatasetFromRestorePoint(const std::string& restore_point_tag,
                                                                                     KeyDataStorageDomain domain);
                
                void updateConfiguration(chaos::common::data::CDataWrapper *configuration);
                void updateConfiguration(const std::string& conf_name, const chaos::common::data::CDataVariant& conf_value);
                
                //!ovveridethe storage type sy into the driver by remote setting
                void setOverrideStorageType(chaos::DataServiceNodeDefinitionType::DSStorageType _override_storage_type);
                
                //!override the check of the time information on live and history data
                void setTimingConfigurationBehaviour(bool _use_timing_info = true);
                
                //! add a tag for all dataset
                void addTag(const std::string& tag);
                void addTag(const ChaosStringSet& tag);
                
                //! remove tag to all dataset
                void removeTag(const std::string& tag);
                void removeTag(const ChaosStringSet& tag);
                
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
                                     const uint64_t end_ts,
                                     const ChaosStringSet& meta_tags);
                
                int performGeneralQuery(chaos::common::io::QueryCursor **cursor,
                                        const std::string& node_id,
                                        KeyDataStorageDomain dataset_domain,
                                        const uint64_t start_ts,
                                        const uint64_t end_ts,
                                        const ChaosStringSet& meta_tags);
                
                void releseQuery(chaos::common::io::QueryCursor *cursor);
            };
        }
    }
}
#endif
