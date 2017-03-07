/*
 *	LogWorker.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/03/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__8315DFE_1D95_48D8_A408_7C4B0D444477_LogWorker_h
#define __CHAOSFramework__8315DFE_1D95_48D8_A408_7C4B0D444477_LogWorker_h

#include "../AbstractWorker.h"

#include <chaos_service_common/data/node/Agent.h>

#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/LockableObject.h>

#include <fstream>

namespace chaos {
    namespace agent {
        namespace worker {
            
            //map that containes the file accessor for named pipe
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, boost::shared_ptr<std::ifstream>, MapLoggingPipe);
            
            CHAOS_DEFINE_LOCKABLE_OBJECT(MapLoggingPipe, LockableMapLoggingPipe);
            
            //! define the worker that permit to deploy chaos executable on host
            class LogWorker:
            public AbstractWorker,
            public chaos::common::async_central::TimerHandler {
                LockableMapLoggingPipe map_logging_file;
            protected:
                chaos::common::data::CDataWrapper *starLoggingAssociation(chaos::common::data::CDataWrapper *data,
                                                                          bool& detach);
                
                chaos::common::data::CDataWrapper *stopLoggingAssociation(chaos::common::data::CDataWrapper *data,
                                                                          bool& detach);
                //!inherited by @chaos::common::async_central::TimerHandler
                void timeout();
            public:
                LogWorker();
                ~LogWorker();
                void init(void *data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
            };
        }
    }
}

#endif /* __CHAOSFramework__8315DFE_1D95_48D8_A408_7C4B0D444477_LogWorker_h */
