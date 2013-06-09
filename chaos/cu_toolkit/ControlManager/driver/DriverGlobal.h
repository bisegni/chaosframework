/*
 *	DriverGlobal.h
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

#ifndef CHAOSFramework_DriverGlobal_h
#define CHAOSFramework_DriverGlobal_h

#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/spsc_queue.hpp>

namespace chaos{
    namespace cu {
        namespace cm {
            
            //! The name space that group all foundamental class need by driver implementation on chaos
            namespace driver {
                
                typedef boost::lockfree::queue<int, boost::lockfree::fixed_sized<false> > OutputAccessorQueue, *OutputAccessorQueuePtr;

                typedef struct DrvCmd {
                    uint16_t opcode;
                    uint16_t property;
                    void *data;
                    OutputAccessorQueuePtr *responseQueue;
                } DrvMsg, *DrvMsgPtr;
                
                typedef boost::lockfree::queue<DrvMsgPtr, boost::lockfree::fixed_sized<false> > InputSharedQueue, *InputSharedQueuePtr;

            }
        }
    }
}

#endif
