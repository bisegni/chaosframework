/*
 *	LockableObject.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 19/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_D05B56D9_6CA0_4646_8710_8306D92E3818_LockableObject_h
#define __CHAOSFramework_D05B56D9_6CA0_4646_8710_8306D92E3818_LockableObject_h

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace common {
        namespace utility {
         
            typedef ChaosReadLock   LockableObjectReadLock;
            typedef boost::shared_ptr<LockableObjectReadLock>   LockableObjectReadLockShrdPtr;
            
            typedef ChaosWriteLock LockableObjectWriteLock;
            typedef boost::shared_ptr<LockableObjectWriteLock>   LockableObjectWriteLockShrdPtr;
            
            template<typename T>
            class LockableObject  {
                ChaosSharedMutex mutex_container_dataset;
            public:
                T container_dataset;
                
                void getReadLock(LockableObjectReadLock& read_lock) {
                    read_lock = LockableObjectReadLock(mutex_container_dataset);
                }
                LockableObjectReadLockShrdPtr getReadLockAsSharedPtr() {
                    LockableObjectReadLockShrdPtr(new LockableObjectReadLock(mutex_container_dataset));
                }
                
                void getWriteLock(LockableObjectWriteLock& write_lock) {
                    write_lock = LockableObjectWriteLock(mutex_container_dataset);
                }
                
                LockableObjectWriteLockShrdPtr getWriteLockAsShrdPtr() {
                    return LockableObjectWriteLockShrdPtr(new LockableObjectWriteLock(mutex_container_dataset));
                }
                
                T& operator()(){
                    return container_dataset;
                }
            };
        }
    }
}

#endif /* __CHAOSFramework_D05B56D9_6CA0_4646_8710_8306D92E3818_LockableObject_h */
