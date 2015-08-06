/*
 *	ProcessHello.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__ProcessHello__
#define __CHAOSFramework__ProcessHello__

#include "ChaosMetadataService/api/AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace healt {

                    //! accept the welcom message from a !CHAOS process
                /*!
                 Every chaos node is hosted within a Chaos Common Toolkit singleton instance. So every 
                 instance live within an operating system process. Within the initialization phase the 
                 common toolkit, in a synchronous way, call this api to register itself and to get tree 
                 server where publish healt information.
                 */
                class ProcessHello:
                public AbstractApi {
                public:
                    ProcessHello();
                    ~ProcessHello();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__ProcessHello__) */
