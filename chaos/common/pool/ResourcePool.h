
/*
 *	ResourcePool.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright <#date#> INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__ResourcePool__
#define __CHAOSFramework__ResourcePool__

#include <chaos/common/global.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/TimingUtil.h>

namespace chaos {
    namespace common {
        namespace pool {
            
#define CHAOS_RESOURCE_POOL_DELETE_SLOT(x)\
resource_pooler_helper->deallocateResource(pool_identity, x->resource_pooled);\
delete(x);
            
            //! Abstract resource pool manager
            /*!
             Resource pool abstract class permit to create a pool manager for an abstract resource type
             The creation and release of a resource is demanded to an implementation of the abastract class
             @ResourcePool::ResourcePoolHelper
             */
            template<typename R>
            class ResourcePool {
            public:
                //!helper class for resource managin
                /*!
                    Subclass need to extends this class to permit the manage the creation and deallocation 
                 of the resource
                */
                class ResourcePoolHelper {
                    friend class ResourcePool;
                protected:
                    //! allocate a new resource
                    virtual R allocateResource(const std::string& pool_identification, uint32_t& alive_for_ms) = 0;
                    //!deallocate a resource
                    virtual void deallocateResource(const std::string& pool_identification, R resource_to_deallocate) = 0;
                };
                
                //!Resource pool slot
                /*!
                 Identify a slot with an already created resource that is available into the pool.
                 Every slot is a time-to-live after the wich, when is possible, it will be free.
                 */
                struct ResourceSlot {
                    R resource_pooled;
                    //the timestamp in milliseconds (according to chaos timestamp) that this slot is valid
                    //! after that time, when it is possible the slot will be removed
                    uint64_t valid_until;
                    const std::string pool_identification;
                    ResourceSlot(const std::string& _pool_identification,
                                 R _resource_pooled):
                    pool_identification(_pool_identification),
                    resource_pooled(_resource_pooled){}
                    ~ResourceSlot() {}
                };
                

                //!Resource pool
                ResourcePool(const std::string& _pool_identity,
                             ResourcePoolHelper *_resource_pooler_helper):
                pool_identity(_pool_identity),
                resource_pooler_helper(_resource_pooler_helper){
                }
                
                ~ResourcePool() {
                    boost::unique_lock<boost::mutex> l(mutex_r_pool);
                    for(typename std::deque< ResourceSlot* >::iterator it = r_pool.begin();
                        it!=r_pool.end();
                        it++) {
                        //check and delete resource and slot
                        CHAOS_RESOURCE_POOL_DELETE_SLOT((*it))
                    }
                    
                }
                //!get a new resource from the pool
                /*!
                 Try to retun an already allocated resurce from pool. It the pool is empy
                 it provide the allcoation of a new resource and return it
                 */
                ResourceSlot *getNewResource() {
                    uint32_t alive_for_ms;
                    boost::unique_lock<boost::mutex> l(mutex_r_pool);
                    ResourceSlot *resource_slot_ptr = NULL;
                    if(r_pool.empty()) {
                        //create temporare autoPtr for safe operation in case of exception
                        std::auto_ptr<ResourceSlot> _temp_resource_lot;
                        try {
                            _temp_resource_lot.reset(new ResourceSlot(pool_identity,
                                                                      resource_pooler_helper->allocateResource(pool_identity,
                                                                                                               alive_for_ms)));
                            if(_temp_resource_lot->resource_pooled) {
                                //we have a valid resource wo we need to set his liveness
                                _temp_resource_lot->valid_until = chaos::common::utility::TimingUtil::getTimeStamp() + alive_for_ms;

                            }
                        } catch (...) {}
                        
                        if(_temp_resource_lot->resource_pooled == NULL) {
                            //error creating resource
                        } else {
                            //all is gone well so we can release the temp smatr pointer to result pointer
                            resource_slot_ptr = _temp_resource_lot.release();
                        }
                    } else {
                        //return alread allcoated one
                        resource_slot_ptr = r_pool.front();
                        //remove associated pointr
                        r_pool.pop_front();
                    }
                    return resource_slot_ptr;
                }
                
                //! return a resurce to the socket
                void releaseResource(ResourceSlot *resource_slot) {
                    boost::unique_lock<boost::mutex> l(mutex_r_pool);
                    r_pool.push_front(resource_slot);
                }
                
                size_t getSize() {
                    boost::unique_lock<boost::mutex> l(mutex_r_pool);
                    return r_pool.size();
                }
                
                void maintenance() {
                    //lock pool
                    boost::unique_lock<boost::mutex> l(mutex_r_pool);
                    //check maximum 3 element
                    int max_to_check = r_pool.size()>3?3:(int)r_pool.size();
                    //check the current ts in ms
                    uint64_t now_in_ms = chaos::common::utility::TimingUtil::getTimeStamp();
                    
                    while((max_to_check--)> 0) {
                        ResourceSlot *resource_slot = r_pool.back();
                        if(resource_slot->valid_until < now_in_ms) {
                            //slot no more valid
                            r_pool.pop_back();
                            //check and delete resource and slot
                            CHAOS_RESOURCE_POOL_DELETE_SLOT(resource_slot)
                        } else {
                            break;
                        }
                    }
                }
                
            private:
                typedef std::deque< ResourceSlot* > SocketPoolQueue;
                //pool identification
                const std::string pool_identity;
                ResourcePoolHelper *resource_pooler_helper;
                
                //pool queue structure
                boost::mutex mutex_r_pool;
                SocketPoolQueue r_pool;
            };
        }
    }
}
#endif /* defined(__CHAOSFramework__ResourcePool__) */
