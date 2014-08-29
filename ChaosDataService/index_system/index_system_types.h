/*
 *	index_systsem_types.h
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
#ifndef CHAOSFramework_index_system_types_h
#define CHAOSFramework_index_system_types_h

#include <string>
#include <stdint.h>

#include "../vfs/DataBlock.h"

namespace chaos {
	namespace data_service {
		namespace index_system {
			
			//! definition of the datapack index
			/*!
			 Structure for the managment of the datapack index
			 */
			struct DataPackIndex {
				//! unique identification of the instrumentation that has produced the data
				std::string did;
				
				//! the timestamp that identifying the time qhen the data pack has been collected
				uint64_t	acquisition_ts;
				
				//! location whithin the destination file
				chaos::data_service::vfs::FileLocationPointer dst_location;
			};
			
		}
	}
}

#endif
