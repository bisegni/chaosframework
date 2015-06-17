/*
 *	UnitServerApiGroup.h
 *	!CHAOS
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
#ifndef __CHAOSFramework__UnitServerApiGroup__
#define __CHAOSFramework__UnitServerApiGroup__

#include "ChaosMetadataService/api/AbstractApiGroup.h"

namespace chaos {
	namespace metadata_service {
		namespace api {
			namespace unit_server {
				
				//! api group for the managment of the producer
				DECLARE_CLASS_FACTORY(UnitServerApiGroup, AbstractApiGroup) {
					REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(UnitServerApiGroup)
				public:
					UnitServerApiGroup();
					~UnitServerApiGroup();
				};
				
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__ProducerGroup__) */
