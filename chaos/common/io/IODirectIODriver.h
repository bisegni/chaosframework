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

#ifndef __CHAOSFramework__IODirectIODriver__
#define __CHAOSFramework__IODirectIODriver__

#include <set>
#include <map>
#include <string>

#include <chaos/common/chaos_types.h>
#include <chaos/common/io/IODataDriver.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>
#include <chaos/common/direct_io/channel/DirectIOSystemAPIClientChannel.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>
#include <chaos/common/direct_io/channel/DirectIODeviceServerChannel.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/ObjectSlot.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/network/URLServiceFeeder.h>
#include <chaos/common/async_central/async_central.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>


namespace chaos{
    namespace common {
        namespace io {
            typedef struct IODirectIODriverClientChannels {
                chaos::common::direct_io::DirectIOClientConnection			        *connection;
                chaos::common::direct_io::channel::DirectIODeviceClientChannel		*device_client_channel;
                chaos::common::direct_io::channel::DirectIOSystemAPIClientChannel	*system_client_channel;
            } IODirectIODriverClientChannels;
            
            //!define the info for the evition management of the url
            struct OfflineUrlEvicInfo {
                const uint32_t url_index;
                const std::string url;
                const int64_t evict_ts;
                
                OfflineUrlEvicInfo(uint32_t _url_index, const std::string& _url, int64_t timeout):
                url_index(_url_index),
                url(_url),
                evict_ts(chaos::common::utility::TimingUtil::getTimestampWithDelay(timeout, true)){}
            };
            
            CHAOS_DEFINE_MAP_FOR_TYPE(uint32_t, ChaosSharedPtr<OfflineUrlEvicInfo>, MapUrlEvition);
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapUrlEvition, LMapUrlEvition);
            /*!
             */
            DECLARE_CLASS_FACTORY(IODirectIODriver, IODataDriver),
            public chaos::common::utility::NamedService,
            public common::network::URLServiceFeederHandler,
            protected chaos::common::direct_io::DirectIOClientConnectionEventHandler,
            protected chaos::common::async_central::TimerHandler {
            public:
                //!define the handler for received the eviction event
                typedef ChaosFunction<void(const ChaosStringSet&/*evicted url*/)> EvictionUrlHandler;
            private:
                REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(IODirectIODriver)
                std::set<std::string> registered_server;
                
                const bool evict_dead_url;
                const int32_t dead_url_timeout;
                LMapUrlEvition map_url_eviction;
                
                chaos::common::direct_io::DirectIOClient    *client_instance;
                chaos::common::utility::ObjectSlot<IODirectIODriverClientChannels*>	channels_slot;
                
                WaitSemaphore wait_get_answer;
                ChaosSharedMutex mutext_feeder;
                
                //query future management
                ChaosSharedMutex                    map_query_future_mutex;
                std::map<std::string, QueryCursor*>	map_query_future;
                
                const std::string uuid;
                bool shutting_down;
                EvictionUrlHandler eviction_handler;
            protected:
                ChaosSharedMutex push_mutex;
                chaos::common::network::URLServiceFeeder connectionFeeder;
                //!inherited by @chaos::common::async_central::TimerHandler
                void timeout();
                //!inherited by @common::network::URLServiceFeederHandler
                void disposeService(void *service_ptr);
                 //!inherited by @common::network::URLServiceFeederHandler
                void* serviceForURL(const common::network::URL& url, uint32_t service_index);
                 //!inherited by @chaos::common::direct_io::DirectIOClientConnectionEventHandler
                void handleEvent(chaos::common::direct_io::DirectIOClientConnection *client_connection,
                                 chaos::common::direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event);
            public:
                IODirectIODriver(const std::string& alias);
                virtual ~IODirectIODriver();
                
                void addServerURL(const std::string& url);
                
                void removeServerURL(const std::string& url);
                
                void setEvictionHandler(EvictionUrlHandler _handler);
                
                const std::string& getUUID() const;
                /*
                 * Init method
                 */
                void init(void *init_parameter) throw(CException);
                
                /*
                 * Deinit method
                 */
                void deinit() throw(CException);
                
                int storeHealthData(const std::string& key,
                                     chaos::common::data::CDWShrdPtr data_to_store,
                                     DataServiceNodeDefinitionType::DSStorageType storage_type,
                                     const ChaosStringSet& tag_set = ChaosStringSet()) throw(CException);
                
                /*
                 * storeRawData
                 */
                int storeData(const std::string& key,
                               chaos::common::data::CDWShrdPtr data_to_store,
                               DataServiceNodeDefinitionType::DSStorageType storage_type,
                               const ChaosStringSet& tag_set = ChaosStringSet())  throw(CException);
                
                int removeData(const std::string& key,
                               uint64_t start_ts,
                               uint64_t end_ts) throw(CException);
                int retriveMultipleData(const ChaosStringVector& key,
                                        chaos::common::data::VectorCDWShrdPtr& result)  throw(CException);
                /*
                 * retriveRawData
                 */
                char * retriveRawData(const std::string& key,
                                      size_t *dim=NULL)  throw(CException);
                
                //! restore from a tag a dataset associated to a key
                int loadDatasetTypeFromSnapshotTag(const std::string& restore_point_tag_name,
                                                   const std::string& key,
                                                   uint32_t dataset_type,
                                                   chaos::common::data::CDWShrdPtr& cdw_shrd_ptr);
                
                /*
                 * updateConfiguration
                 */
                chaos::common::data::CDataWrapper* updateConfiguration(chaos::common::data::CDataWrapper*);

                QueryCursor* performQuery(const std::string& key,
                                          uint64_t           start_ts,
                                          uint64_t           end_ts,
                                          uint32_t           page = DEFAULT_PAGE_LEN);

                QueryCursor* performQuery(const std::string&    key,
                                          uint64_t              start_ts,
                                          uint64_t              end_ts,
                                          const ChaosStringSet& meta_tags = ChaosStringSet(),
                                          uint32_t              page      = DEFAULT_PAGE_LEN);

                QueryCursor* performQuery(const std::string& key,
                                          uint64_t           start_ts,
                                          uint64_t           end_ts,
                                          uint64_t           sequid,
                                          uint64_t           runid,
                                          uint32_t           page = DEFAULT_PAGE_LEN);

                QueryCursor* performQuery(const std::string&    key,
                                          uint64_t              start_ts,
                                          uint64_t              end_ts,
                                          uint64_t              sequid,
                                          uint64_t              runid,
                                          const ChaosStringSet& meta_tags = ChaosStringSet(),
                                          uint32_t              page      = DEFAULT_PAGE_LEN);

                void releaseQuery(QueryCursor *query_cursor);
            };
        }
    }
}


#endif /* defined(__CHAOSFramework__IODirectIODriver__) */
