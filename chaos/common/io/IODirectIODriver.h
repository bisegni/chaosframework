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
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/network/URLServiceFeeder.h>

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

namespace chaos_data = chaos::common::data;
namespace chaos_utility = chaos::common::utility;
namespace chaos_direct_io = chaos::common::direct_io;
namespace chaos_dio_channel = chaos::common::direct_io::channel;

namespace chaos{
    namespace common {
        namespace io {
            
            /*!
             Struct for initialization of the io driver
             */
            typedef struct IODirectIODriverInitParam {
                chaos_direct_io::DirectIOClient			*client_instance;
                chaos_direct_io::DirectIOServerEndpoint *endpoint_instance;
            } IODirectIODriverInitParam, *IODirectIODriverInitParamPtr;
            
            
            typedef struct IODData {
                void *data_ptr;
                uint32_t data_len;
            } IODData;
            
            typedef struct IODirectIODriverClientChannels {
                chaos_direct_io::DirectIOClientConnection			*connection;
                chaos_dio_channel::DirectIODeviceClientChannel		*device_client_channel;
                chaos_dio_channel::DirectIOSystemAPIClientChannel	*system_client_channel;
            } IODirectIODriverClientChannels;
            
            
            /*!
             */
            DECLARE_CLASS_FACTORY(IODirectIODriver, IODataDriver),
            public chaos::common::utility::NamedService,
            public common::network::URLServiceFeederHandler,
            private chaos_dio_channel::DirectIODeviceServerChannel::DirectIODeviceServerChannelHandler,
            protected chaos_direct_io::DirectIOClientConnectionEventHandler {
                REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(IODirectIODriver)
                IODirectIODriverInitParam init_parameter;
                std::set<std::string> registered_server;
                
                uint16_t	current_endpoint_p_port;
                uint16_t	current_endpoint_s_port;
                uint16_t	current_endpoint_index;
                
                chaos_dio_channel::DirectIODeviceServerChannel				*device_server_channel;
                chaos_utility::ObjectSlot<IODirectIODriverClientChannels*>	channels_slot;
                
                WaitSemaphore wait_get_answer;
                
                //query future management
                ChaosSharedMutex                    map_query_future_mutex;
                std::map<std::string, QueryCursor*>	map_query_future;
                
                std::string uuid;
                bool use_index;
                bool shutting_down;
            protected:
                ChaosSharedMutex push_mutex;
                ChaosSharedMutex mutext_feeder;
                chaos::common::network::URLServiceFeeder connectionFeeder;
                void disposeService(void *service_ptr);
                void* serviceForURL(const common::network::URL& url, uint32_t service_index);
                void handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
                                 chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event);
            public:
                
                IODirectIODriver(const std::string& alias);
                virtual ~IODirectIODriver();
                
                void setDirectIOParam(IODirectIODriverInitParam& _init_parameter);
                
                void addServerURL(const std::string& url);
                /*
                 * Init method
                 */
                void init(void *init_parameter);
                
                /*
                 * Deinit method
                 */
                void deinit();
                
                /**
                 *
                 */
                int storeHealthData(const std::string& key,
                                     chaos_data::CDWShrdPtr data_to_store,
                                     DataServiceNodeDefinitionType::DSStorageType storage_type,
                                     const ChaosStringSet& tag_set = ChaosStringSet());
                
                /*
                 * storeRawData
                 */
                int storeData(const std::string& key,
                               chaos_data::CDWShrdPtr data_to_store,
                               DataServiceNodeDefinitionType::DSStorageType storage_type,
                               const ChaosStringSet& tag_set = ChaosStringSet());
                /**
                 *
                 */
                int removeData(const std::string& key,
                               uint64_t start_ts,
                               uint64_t end_ts);
                
                /**
                 *
                 */
                int retriveMultipleData(const ChaosStringVector& key,
                                        chaos::common::data::VectorCDWShrdPtr& result);
                
                /*
                 * retriveRawData
                 */
                char * retriveRawData(const std::string& key,
                                      size_t *dim=NULL);
                
                //! restore from a tag a dataset associated to a key
                int loadDatasetTypeFromSnapshotTag(const std::string& restore_point_tag_name,
                                                   const std::string& key,
                                                   uint32_t dataset_type,
                                                   chaos_data::CDWShrdPtr& cdw_shrd_ptr);
                
                /*
                 * updateConfiguration
                 */
                chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*);
                
                void setQueryOnIndex(bool _use_index);
                
                /**
                 *
                 */
                QueryCursor* performQuery(const std::string& key,
                                          uint64_t           start_ts,
                                          uint64_t           end_ts,
                                          uint32_t           page = DEFAULT_PAGE_LEN);

                QueryCursor* performQuery(const std::string&    key,
                                          uint64_t              start_ts,
                                          uint64_t              end_ts,
                                          const ChaosStringSet& meta_tags = ChaosStringSet(),
                                          const ChaosStringSet& projection_keys = ChaosStringSet(),
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
                                          const ChaosStringSet& projection_keys = ChaosStringSet(),
                                          uint32_t              page      = DEFAULT_PAGE_LEN);

                void releaseQuery(QueryCursor *query_cursor);
            };
        }
    }
}


#endif /* defined(__CHAOSFramework__IODirectIODriver__) */
