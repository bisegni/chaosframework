/*
 *	DeviceApi.h
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

#ifndef __CHAOSFramework__DeviceApi__
#define __CHAOSFramework__DeviceApi__

#include "ApiProvider.h"

namespace chaos_data = chaos::common::data;

namespace chaos {
    namespace cnd {
        namespace api {
            
            /*!
             
             */
            class DeviceApi: public ApiProvider {
               
                
            public:
                DeviceApi();
                ~DeviceApi();
                
                /*!
                 Register a device
                 */
				chaos_data::CDataWrapper* registerDevice(chaos_data::CDataWrapper *registrationdData, bool& detachParam) throw(CException);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__DeviceApi__) */
