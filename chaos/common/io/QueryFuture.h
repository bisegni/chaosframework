//
//  QueryFuture.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 11/09/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__QueryFuture__
#define __CHAOSFramework__QueryFuture__

#include <string>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/thread/WaitSemaphore.h>

#include <boost/lockfree/queue.hpp>

namespace cc_data = chaos::common::data;

namespace chaos {
	namespace common {
		namespace io {
			
			class IODataDriver;
			
			//! is the result for a query
			/*!
			 waith and return the result of a query in a synchronous and asynchronous way
			 */
			class QueryFuture {
				friend class IODataDriver;
				
				std::string query_id;
				WaitSemaphore waith_for_get_data_Semaphore;
				WaitSemaphore waith_for_push_data_Semaphore;
				
				//datapack result of query
				boost::lockfree::queue<cc_data::CDataWrapper*, boost::lockfree::fixed_sized<false> > data_pack_queue;
				
				QueryFuture(const std::string& _query_id);
				
				virtual ~QueryFuture();
				
				void pushDataPack(cc_data::CDataWrapper *received_datapack);
			public:
				cc_data::CDataWrapper *getDataPack(bool wait = true);
				
				const std::string& getQueryID();
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__QueryFuture__) */
