/*
 *	DriverAccessor.cpp
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

#ifndef __CHAOSFramework__DriverAccessor__
#define __CHAOSFramework__DriverAccessor__

#include <chaos/cu_toolkit/ControlManager/driver/DriverGlobal.h>

namespace chaos{
    
    //! forward declaration
    template <typename T>
    class PriorityQueuedElement;
    
    namespace cu {
        namespace cm {
            namespace driver {
                
                class DriverAccessor {
                    friend class AbstractDriver;
   
                    //! input shared queue
                    InputSharedQueuePtr inputQueue;
                    
                    //! output queue
                    OutputAccessorQueuePtr outputQueue;
                    
                    //Private constructor
                    DriverAccessor();
                    
                    //Private destructor
                    ~DriverAccessor();
                public:

                    
                    int read();
                    
                    int writeOpcode(uint16_t opcode, uint16_t property, void *data, uint16_t& cmdCode);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DriverAccessor__) */
