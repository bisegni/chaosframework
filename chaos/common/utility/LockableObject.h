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
            
            typedef ChaosReadLock   LockableObjectReadLock_t;
            typedef ChaosWriteLock  LockableObjectWriteLock_t;
            
#define CHAOS_DEFINE_LOCKABLE_OBJECT(x, n)\
typedef chaos::common::utility::LockableObject<x> n;\
typedef chaos::common::utility::LockableObject<x>::LockableObjectReadLock n ## ReadLock;\
typedef chaos::common::utility::LockableObject<x>::LockableObjectWriteLock n ## WriteLock;\

            
            template<typename T>
            class LockableObject  {
                ChaosSharedMutex mutex_container_dataset;
            public:
                
                LockableObject(){}
                
                LockableObject(const T& src){
                    container_object = src;
                }
                
                //!readable lock class
                class ReadLock {
                    friend class LockableObject<T>;
                    LockableObjectReadLock_t rl;
                    ReadLock(LockableObject<T>& lockable_obj_ref) {
                        lockable_obj_ref.getReadLock(rl);
                    }
                };
                typedef boost::shared_ptr<ReadLock> LockableObjectReadLock;
                
                //!writeable lock class
                class WriteLock {
                    friend class LockableObject<T>;
                    LockableObjectWriteLock_t wl;
                    WriteLock(LockableObject<T>& lockable_obj_ref) {
                        lockable_obj_ref.getWriteLock(wl);
                    }
                };
                typedef boost::shared_ptr<WriteLock> LockableObjectWriteLock;
                
                T container_object;
                
                void getReadLock(LockableObjectReadLock_t& read_lock) {
                    read_lock = LockableObjectReadLock_t(mutex_container_dataset);
                }
                LockableObjectReadLock getReadLockObject() {
                    return LockableObjectReadLock(new ReadLock(*this));
                }
                
                void getWriteLock(LockableObjectWriteLock_t& write_lock) {
                    write_lock = LockableObjectWriteLock_t(mutex_container_dataset);
                }
                
                LockableObjectWriteLock getWriteLockObject() {
                    return LockableObjectWriteLock(new WriteLock(*this));
                }
                
                T& operator()(){
                    return container_object;
                }
                
                inline bool operator==(const T& lhs) {
                    return container_object == lhs;
                }
                
                inline bool operator!=(const T& lhs) {
                    return container_object != lhs;
                }
            };
        }
    }
}

#endif /* __CHAOSFramework_D05B56D9_6CA0_4646_8710_8306D92E3818_LockableObject_h */
