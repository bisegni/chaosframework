/*
 *	IODirectIODriver.h
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
            using namespace std;
            using namespace boost;

            /*!
             Struct for initialization of the io driver
             */
            typedef struct IODirectIODriverInitParam {
                chaos::common::network::NetworkBroker	*network_broker;
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
                boost::shared_mutex mutext_feeder;
                
                IODData data_cache;
                chaos::common::network::URLServiceFeeder connectionFeeder;
                
                //query future management
                boost::shared_mutex				map_query_future_mutex;
                std::map<std::string, QueryCursor*>	map_query_future;
                
                std::string uuid;
            protected:
                void disposeService(void *service_ptr);
                void* serviceForURL(const common::network::URL& url, uint32_t service_index);
                void handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
                                 chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event);
            public:
                
                IODirectIODriver(std::string alias);
                virtual ~IODirectIODriver();
                
                void setDirectIOParam(IODirectIODriverInitParam& _init_parameter);
                
                void addServerURL(const std::string& url);
                /*
                 * Init method
                 */
                void init(void *init_parameter) throw(CException);
                
                /*
                 * Deinit method
                 */
                void deinit() throw(CException);
                
                /*
                 * storeRawData
                 */
                void storeRawData(const std::string& key,
                                  chaos_data::SerializationBuffer *serialization,
                                  DataServiceNodeDefinitionType::DSStorageType storage_type)  throw(CException);
                
                int removeData(const std::string& key,
                               uint64_t start_ts,
                               uint64_t end_ts) throw(CException);
                
                /*
                 * retriveRawData
                 */
                char * retriveRawData(const std::string& key,
                                      size_t *dim=NULL)  throw(CException);
                
                //! restore from a tag a dataset associated to a key
                int loadDatasetTypeFromSnapshotTag(const std::string& restore_point_tag_name,
                                                   const std::string& key,
                                                   uint32_t dataset_type,
                                                   chaos_data::CDataWrapper **cdatawrapper_handler);
                //! restore from a tag a dataset associated to a key
                int createNewSnapshot(const std::string& snapshot_tag,
                                      const std::vector<std::string>& node_list);
                
                int deleteSnapshot(const std::string& snapshot_tag);
                
                /*
                 * updateConfiguration
                 */
                chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*);
                
                QueryCursor *performQuery(const std::string& key,
                                          uint64_t start_ts,
                                          uint64_t end_ts,uint32_t page=DEFAULT_PAGE_LEN);
                
                void releaseQuery(QueryCursor *query_cursor);
            };
        }
    }
}


#endif /* defined(__CHAOSFramework__IODirectIODriver__) */
