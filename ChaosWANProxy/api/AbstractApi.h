/*
 *	AbstractApi.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__AbstractApi__
#define __CHAOSFramework__AbstractApi__

#include <chaos/common/utility/NamedService.h>

#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/recursive_mutex.hpp>
namespace chaos {
	namespace wan_proxy {
		namespace api {
			
			//! abstract api class definition
			class AbstractApi:
			public chaos::common::utility::NamedService,
			//! permit to the Api class to be used as mutex
			public boost::basic_lockable_adapter<boost::recursive_mutex> {
			public:
				AbstractApi(const std::string& name);
				virtual ~AbstractApi();
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__AbstractApi__) */
