/*
 *	IODataDriver.h
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

#ifndef IODataDriver_H
#define IODataDriver_H
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <chaos/common/exception/CException.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/utility/ArrayPointer.h>
#include <chaos/common/io/QueryFuture.h>

namespace chaos_data = chaos::common::data;

namespace chaos{
	namespace common {
		namespace io {
			
			/*!
			 * History Output driver base abstract classe, that define two method to control
			 * the initialization and history persistence of the data
			 */
			
			class IODataDriver: public Configurable {
			protected:
				std::string dataKey;
				
				QueryFuture *_getNewQueryFutureForQueryID(const std::string& query_id);
				
				void _releaseQueryFuture(QueryFuture *query_future_ptr);
				
				void _pushDataToQuryFuture(QueryFuture& query_future, chaos_data::CDataWrapper *data_pack);
			public:
				virtual ~IODataDriver(){};
				/*!
				 * Init method, the has map has all received value for configuration
				 * every implemented driver need to get all needed configuration param
				 */
				virtual void init(void *init_parameter) throw(CException);
				/*!
				 * DeInit method
				 */
				virtual void deinit() throw(CException);
				
				/*!
				 * This method cache all object passed to driver
				 */
				void storeData(chaos_data::CDataWrapper *dataToStore) throw(CException);
				
				/*!
				 * This method retrive the cached object by CSDawrapperUsed as query key and
				 * return a pointer to the class ArrayPointer of CDataWrapper type
				 */
				virtual ArrayPointer<chaos_data::CDataWrapper>* retriveData(chaos_data::CDataWrapper*const)  throw(CException);
				
				/*!
				 * This method retrive the cached object by CSDawrapperUsed as query key and
				 * return a pointer to the class ArrayPointer of CDataWrapper type
				 */
				virtual ArrayPointer<chaos_data::CDataWrapper>* retriveData()  throw(CException);
				
				/*!
				 * This method store a buffer into live cached
				 */
				virtual void storeRawData(chaos_data::SerializationBuffer *serialization)  throw(CException) = 0;
				
				/*!
				 * This method retrive the cached object by CSDawrapperUsed as query key and
				 * return a pointer to the class ArrayPointer of CDataWrapper type
				 */
				virtual char * retriveRawData(size_t* dataDim=NULL)  throw(CException) = 0;
				
				/*!
				 Update the driver configuration
				 */
				virtual chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*);
				
				
				//! perform a query since and
				virtual QueryFuture *performQuery(uint64_t start_ts, uint64_t end_ts);
				
				//! close a query 
				virtual void releaseQuery(QueryFuture *query_future);
			};
			
		}
	}
}
#endif
