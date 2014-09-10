/*
 *	QueryAnswerEngine.h
 *	!CHOAS
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
#ifndef __CHAOSFramework__QueryAnswerEngine__
#define __CHAOSFramework__QueryAnswerEngine__

#include "../vfs/vfs.h"

#include <chaos/common/utility/StartableService.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/utility/TemplatedKeyValueHash.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lockfree/queue.hpp>

namespace chaos_direct_io = chaos::common::direct_io;
namespace chaos_direct_io_ch = chaos::common::direct_io::channel;

namespace chaos {
	namespace data_service {
		namespace query_engine {
			class QueryEngine;
			
			//! Class that manage the query at higer level
			/*!
			 This is the root class that perform and answer to a query request. The query and the forwarding are done in scheduling way.
			 A thread pool is created that perform a "step" of a query. A step is or a real query execution of a forward of a "page" of a result,
			 then another query is "Stepped". Wuen a query has not more page it is deleted and the connection to the client is closed
			 */
			class DataCloudQuery {
				friend class QueryEngine;
				
				//! the phase of query
				typedef enum DataCloudQueryPhase {
					DataCloudQueryPhaseEmpty = 0,		/**< The query is empty and not configured */
					DataCloudQueryPhaseNeedSearch,		/**< the query need to phisically execute the qeury on index database */
					DataCloudQueryPhaseHaveData,		/**< the query have done the query and can begin to fetch data */
					DataCloudQueryPhaseError,			/**< the query is in error */
					DataCloudQueryPhaseEnd				/**< the query has ended */
				} DataCloudQueryPhase;
				
				//!reflect the associated connection state
				chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType answer_connection_state;
				
				//! the phase of query
				DataCloudQueryPhase query_phase;
				
				//! query id
				std::string query_id;
				
				//! DirectIO endpoint where push the found data
				std::string answer_endpoint;
				
				//! query unique id (endpoint_queryid)
				std::string query_computed_unique_id;
				
				//! query information
				db_system::DataPackIndexQuery query;

				//! total data pack sent
				uint32_t total_data_pack_sent;
				
				//! vfs query object that abstract the real query on inde and data fetch
				vfs::VFSQuery *vfs_query;
			public:
				DataCloudQuery(const std::string& _query_id,
							   const db_system::DataPackIndexQuery& _query,
							   const std::string& _answer_endpoint);
				~DataCloudQuery();
				
				int startQuery();
				
				int fecthData(std::vector<vfs::FoundDataPack>& found_data_pack, unsigned int element_to_fecth);
			};
			
			
			struct ClientConnectionInfo {
				chaos_direct_io::DirectIOClientConnection		*connection;
				chaos_direct_io_ch::DirectIODeviceClientChannel *channel;
				DataCloudQuery									*query;
				ClientConnectionInfo();
				~ClientConnectionInfo();
			};
			
			//!hash table superclass type definition
			typedef chaos::common::utility::TemplatedKeyValueHash< ClientConnectionInfo* > DirectIOChannelHashTable;

			
			/*!
			 This class is the central that perform asynchronous answering to the
			 data cloud query
			 */
			class QueryEngine:
			public utility::StartableService,
			protected DirectIOChannelHashTable,
			protected chaos_direct_io::DirectIOClientConnectionEventHandler {
				chaos_direct_io::DirectIOClient *directio_client_instance;
				vfs::VFSManager *vfs_manager_ptr;
				
				bool work_on_query;
				unsigned int thread_pool_size;
				boost::thread_group answer_thread_pool;
				boost::lockfree::queue<DataCloudQuery*, boost::lockfree::fixed_sized<false> > query_queue;
				
				//! send data to the requester
				int  getChannelForQuery(const DataCloudQuery& query,
										ClientConnectionInfo **connection_info_handle);
				//send data to client
				int  sendDataToClient(DataCloudQuery& query,
									  const std::vector<vfs::FoundDataPack>& data);
				
				//!process the signle query step
				void process_query();
				
				//!virtual method of hastable DirectIOChannelHashTable
				void clearHashTableElement(const void *key, uint32_t key_len, ClientConnectionInfo *element);
				
				//!virtual function of DirectIOClientConnectionEventHandler
				void handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
								 chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event);
				
				void disposeClientConnectionInfo(ClientConnectionInfo *client_info);
			public:
				
				QueryEngine(chaos_direct_io::DirectIOClient *_directio_client_instance,
							unsigned int _thread_pool_size,
							vfs::VFSManager *_vfs_manager_ptr);
				~QueryEngine();
				
				void init(void *init_data)  throw(CException);
				void start() throw(CException);
				void stop() throw(CException);
				void deinit() throw(CException);
				//execute a query and manage the result
				void executeQuery(DataCloudQuery *query);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__QueryAnswerEngine__) */
