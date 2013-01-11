/*
 *	MemoryAllocator.h
 *	!CHOAS
 *	Created by Antonucci Flaminio.
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

#ifndef CachingSystem_MemoryAllocator_h
#define CachingSystem_MemoryAllocator_h
//#include <boost/interprocess/allocators/cached_node_allocator.hpp>
#include <boost/pool/pool_alloc.hpp>

namespace chaos {
    namespace caching_system{
        template <typename T>
        class MemoryAllocator {
            boost::fast_pool_allocator<T>* allocatore;
            // std::allocator<T>* allocatore;
            
        public:
            
            MemoryAllocator(){
                allocatore=new boost::fast_pool_allocator<T >();
                //   allocatore=new std::allocator<T>();
                
            }
            
            ~MemoryAllocator(){
                delete allocatore;
                
            }
            inline T* allocate(){
                T* ritorno;
                ritorno =allocatore->allocate(1);
                allocatore->construct(ritorno, T());
                
                return ritorno;
            }
            
            inline void deallocate(T* pointer){
                allocatore->destroy(pointer);
                allocatore->deallocate(pointer,1);
                // delete pointer;
            }
            
        };
    }
}
#endif
