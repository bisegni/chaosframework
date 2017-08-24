/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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
                public:
                    void lock(){rl.lock();}
                    void unlock(){rl.unlock();}
                };
                typedef ChaosSharedPtr<ReadLock> LockableObjectReadLock;
                
                //!writeable lock class
                class WriteLock {
                    friend class LockableObject<T>;
                    LockableObjectWriteLock_t wl;
                    WriteLock(LockableObject<T>& lockable_obj_ref) {
                        lockable_obj_ref.getWriteLock(wl);
                    }
                public:
                    void lock(){wl.lock();}
                    void unlock(){wl.unlock();}
                };
                typedef ChaosSharedPtr<WriteLock> LockableObjectWriteLock;
                
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
