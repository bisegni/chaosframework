/*
 *	IOMemcachedIODriver.h
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

#ifndef IOMemcachedIODriver_H
#define IOMemcachedIODriver_H

#include <string>
#include <libmemcached/memcached.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/NamedService.h>

#include "IODataDriver.h"

namespace chaos{
	namespace common {
		namespace io {
			using namespace std;
			using namespace boost;
			
			namespace chaos_data = chaos::common::data;
			REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(IOMemcachedIODriver, IODataDriver), public NamedService {
				REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(IOMemcachedIODriver)
				boost::mutex useMCMutex;
				memcached_st *memClient;
			public:
				
				IOMemcachedIODriver(std::string alias);
				virtual ~IOMemcachedIODriver();
				
				/*
				 * Init method, the has map has all received value for configuration
				 * every implemented driver need to get all needed configuration param
				 */
				void init(void *init_parameter) throw(CException);
				
				void deinit() throw(CException);
				
				/*
				 * This method retrive the cached object by CSDawrapperUsed as query key and
				 * return a pointer to the class ArrayPointer of CDataWrapper type
				 */
				virtual void storeRawData(chaos_data::SerializationBuffer *serialization)  throw(CException);
				
				/*
				 * This method retrive the cached object by CSDawrapperUsed as query key and
				 * return a pointer to the class ArrayPointer of CDataWrapper type
				 */
				virtual char * retriveRawData(size_t *dim=NULL)  throw(CException);
				/*
				 Update the driver configuration
				 */
				chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*);
			};
		}
	}
}
#endif
