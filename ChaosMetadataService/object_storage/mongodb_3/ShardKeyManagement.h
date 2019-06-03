/*
 * Copyright 2012, 25/05/2018 INFN
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

#ifndef __CHAOSFramework__B2D2009_02A2_468B_9C5C_1D184ECB471F_ShardKeyManagement_h
#define __CHAOSFramework__B2D2009_02A2_468B_9C5C_1D184ECB471F_ShardKeyManagement_h

#include <chaos/common/chaos_types.h>

#include <chaos/common/data/CDataWrapper.h>

#include <chaos/common/async_central/TimerHandler.h>

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/LockableObject.h>

#include <boost/atomic.hpp>
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/random_device.hpp>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>


namespace chaos {
    namespace metadata_service {
        namespace object_storage {
            namespace mongodb_3 {
                
                class DaqZonedInfo {
                    bsoncxx::builder::basic::document index_zone_doc;
                    bsoncxx::builder::basic::document data_zone_doc;
                public:
                    DaqZonedInfo();
                    DaqZonedInfo(DaqZonedInfo&& i) noexcept = default;
                    bsoncxx::builder::basic::document& getIndexDocument();
                    bsoncxx::builder::basic::document& getDataDocument();
                };
                
                
                //!class for the managem of sharding random value for a single key
                class KeyRNDShardInfo {
                    //random generation deifnition
                    static boost::random_device rd;
                    boost::random::uniform_int_distribution<std::uint64_t> rnd_gen_int64;

                    
                    //shard info
                    ChaosSharedMutex lock_mutex;
                    
                    const std::string key;
                    const uint32_t storage_quota;
                    //!next timesstamp after which the check for new rescheduling is done
                    int64_t next_check_ts;
                    //! timestamp after which a new value is calculated laso if storage quota has not been exceeded
                    int64_t next_timeout_ts;
                    //!the quantity of data stored for key
                    uint32_t stored_byte;
                    //!shard key
                    boost::atomic<int64_t> shard_value;
                public:
                    KeyRNDShardInfo(const std::string& _key,
                                    const uint32_t _storage_quota);
                    ~KeyRNDShardInfo();
                    //!return the value to use as shard key
                    /*!
                     the method after a determinated timeout check the quota of data managed by the
                     key. If this quota exceed the preconfigurated one a new value for shard_value
                     variable will be calculated
                     */
                    const int64_t getShardValue(const int64_t now_in_mds,
                                                const uint32_t new_size_byte);
                };
                
                //define map for key shard random information
                CHAOS_DEFINE_MAP_FOR_TYPE(std::string, ChaosSharedPtr<KeyRNDShardInfo>, MapKeyShardInfo);
                CHAOS_DEFINE_LOCKABLE_OBJECT(MapKeyShardInfo, MapKeyShardInfo_L);
                
                //!Class for the managment of the rotating shared key.
                /*!
                 
                 */
                class ShardKeyManagement:
                public chaos::common::utility::Singleton<ShardKeyManagement> {
                    friend chaos::common::utility::Singleton<ShardKeyManagement>;
                    MapKeyShardInfo_L map_key_shard_info;
                    std::string zone_alias;
                public:
                    ShardKeyManagement();
                    ~ShardKeyManagement();
                    
                    //! set the new zone to manage
                    void setZoneAlias(const std::string& new_zone_alias);

                    //!return a new bson object for the managed zone
                    DaqZonedInfo getNewDataPack(const std::string& key,
                                                const int64_t now_in_ms,
                                                const uint32_t new_size_byte);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__B2D2009_02A2_468B_9C5C_1D184ECB470F_ShardKeyManagement_h */
