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
