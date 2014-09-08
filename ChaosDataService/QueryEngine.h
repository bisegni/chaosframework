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

#include "vfs/vfs.h"

#include <chaos/common/utility/StartableService.h>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lockfree/queue.hpp>

namespace chaos {
	    namespace data_service {
			class QueryEngine;
			
		
			//! Class that manage the answer of the query
			/*!
			 This is the root class that perform and answer to a query request. The query and the forwarding are done in scheduling way. 
			 A thread pool is created that perform a "step" of a query. A step is or a real query execution of a forward of a "page" of a result, 
			 then another query is "Stepped". Wuen a query has not more page it is deleted and the connection to the client is closed
			 */
			class DataCloudQuery {
				friend class QueryEngine;
				
				typedef enum DataCloudQueryPhase {
					DataCloudQueryPhaseEmpty = 0,
					DataCloudQueryPhaseSearch,
					DataCloudQueryPhaseAnswer,
					DataCloudQueryPhaseError,
					DataCloudQueryPhaseEnd
				} DataCloudQueryPhase;
				
				DataCloudQueryPhase query_phase;
				//query submission and answer information
				db_system::DataPackIndexQuery query;
				std::string answer_endpoint;
				
				vfs::VFSQuery *vfs_query;
			public:
				DataCloudQuery(const db_system::DataPackIndexQuery& query,
							   const std::string& answer_endpoint):
				query_phase(DataCloudQueryPhaseSearch),
				vfs_query(NULL){};
				~DataCloudQuery(){};
			};
			
			/*!
			 This class is the central singlet that perform asynchronous answering to the 
			 data cloud query
			 */
			class QueryEngine:
			public utility::StartableService {
				
				vfs::VFSManager *vfs_manager_ptr;
				
				bool work_on_query;
				unsigned int thread_pool_size;
				boost::thread_group answer_thread_pool;
				boost::lockfree::queue<DataCloudQuery*, boost::lockfree::fixed_sized<false> > query_queue;
				
				void process_query();
				
			public:
				
				QueryEngine(unsigned int _thread_pool_size,
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

#endif /* defined(__CHAOSFramework__QueryAnswerEngine__) */
