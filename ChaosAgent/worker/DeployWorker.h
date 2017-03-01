/*
 *	DeployWorker.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/03/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__249F04F_EAE8_49F9_9509_0D0A5A72F0A1_DeployWorker_h
#define __CHAOSFramework__249F04F_EAE8_49F9_9509_0D0A5A72F0A1_DeployWorker_h

#include "../AbstractWorker.h"

#include <chaos_service_common/data/node/Agent.h>

namespace chaos {
    namespace agent {
        namespace worker {
            
            //! define the worker that permit to deploy chaos executable on host
            class DeployWorker:
            public AbstractWorker {
                
            protected:
                chaos::common::data::CDataWrapper *initDeploySession(chaos::common::data::CDataWrapper *data,
                                                                       bool& detach);
                
                chaos::common::data::CDataWrapper *uploadDeployChunk(chaos::common::data::CDataWrapper *data,
                                                                       bool& detach);
                
                chaos::common::data::CDataWrapper *endDeploySession(chaos::common::data::CDataWrapper *data,
                                                                         bool& detach);
            public:
                DeployWorker();
                ~DeployWorker();
                void init(void *data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
            };
        }
    }
}

#endif /* __CHAOSFramework__249F04F_EAE8_49F9_9509_0D0A5A72F0A1_DeployWorker_h */
