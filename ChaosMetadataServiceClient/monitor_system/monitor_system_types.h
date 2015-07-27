/*
 *	monitor_system_types.h
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

#ifndef CHAOSFramework_monitor_system_types_h
#define CHAOSFramework_monitor_system_types_h

#include <chaos/common/data/CDataWrapper.h>

#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>

namespace chaos {
    namespace metadata_service_client {
        namespace monitor_system {
            
            //!is the minimal monitoring delay
#define MONITOR_QUANTUM_LENGTH  100//milli-seconds
            
            
            typedef boost::shared_ptr<chaos::common::data::CDataWrapper> KeyValue;
            
            class QuantumSlotConsumer {
            public:
                //!as called every time there is new data for quantum slot
                virtual void quantumSlotHasData(const std::string& key, const KeyValue& value) = 0;
                //! callend every time that data can't be retrieved from data service
                virtual void quantumSlotHasNoData(const std::string& key) = 0;
            };
        }
    }
}

#endif
