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
			
			//! denote the state of the query future
			typedef enum QueryFutureState {
				QueryFutureStateCreated,			/**< Query has been only created */
				QueryFutureStateExecuted,			/**< Query has been submitted to remote endpoint */
				QueryFutureStateStartResult,		/**< Query has received the message for begin the resutl trasmission */
				QueryFutureStateReceivingResult,	/**< Query has receiving the result data */
				QueryFutureStateEndResult			/**< Query has ended*/
			} QueryFutureState;
			
			//! is the result for a query
			/*!
			 waith and return the result of a query in a synchronous and asynchronous way
			 */
			class QueryFuture {
				friend class IODataDriver;
				
				//!query id for this future
				std::string query_id;
				
				//!total number of element of the query
				uint64_t total_found_element;
				
				//!index of the current fetched element
				uint64_t fetched_element_index;
				
				int32_t error;
				
				std::string error_message;
				
				//! query state
				QueryFutureState state;
				
				//!semaphore
				WaitSemaphore waith_for_get_data_Semaphore;
				WaitSemaphore waith_for_begin_result;
				WaitSemaphore waith_for_push_data_Semaphore;
				//datapack result of query
				boost::lockfree::queue<cc_data::CDataWrapper*, boost::lockfree::fixed_sized<false> > data_pack_queue;
				
				QueryFuture(const std::string& _query_id);
				
				virtual ~QueryFuture();
				
				void pushDataPack(cc_data::CDataWrapper *received_datapack, uint64_t _datapack_index);
				void notifyStartResultPhase(uint64_t _total_element_found);
				void notifyEndResultPhase(int32_t _error, const std::string & _error_message);
			public:
				
				void waitForBeginResult(int32_t timeout = -1);
				
				cc_data::CDataWrapper *getDataPack(bool wait = true, uint32_t timeout = 0);
				
				const std::string& getQueryID();
				
				uint64_t getTotalElementFound();
				
				uint64_t getCurrentElementIndex();
				
				int32_t getError();
				
				const std::string& getErrorMessage();
				
				QueryFutureState getState();
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__QueryFuture__) */
