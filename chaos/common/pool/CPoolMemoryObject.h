/*	
 *	CPoolMemoryObject.h
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

#ifndef ChaosFramework_CPoolMemoryObejct_h
#define ChaosFramework_CPoolMemoryObejct_h


#include <chaos/common/global.h>
#include "CPoolMemory.h"

#include <boost/pool/object_pool.hpp>
#include <boost/interprocess/detail/atomic.hpp>
#include <boost/scoped_ptr.hpp>

namespace chaos {
    using namespace boost;
    
    /*
     This class implement two fondamental things:
        1) object memory pool with the help of boost::object_pool
        2) alloc/retain/release model for support deferred object deallocation
            alloc   ->  initialize memory e se the referenceCount integer to 1
            retain  ->  increment atomically the referenceCount by 1
            release ->  decrement the referenceCount by 1 and is it's 0
                        the object is deallocated
     
     For use this class, a custom class can publically extends CPoolMemoryObject, and
     use the alloc, retain and release method to manage class memory with the pool and
     reference count.
        ex: 
        class Test: public CPoolMemoryObject<Test>{
            ...
        }
     
        usage:
                
            Test *testPtr = Test::alloc();  //make the object (ref. count = 1)
            testPtr->retain();              //inc reference count by 1(ref. count = 2)
            testPtr->release();             //dec reference count by 1(ref. count = 1)
            testPtr->release();             //dec reference count by 1(ref. count = 0, object is deleted)
            
     */
    template <typename T>
    class CPoolMemoryObject {
        typedef T* (*AllocDef)();
                
            //internal typed pool mamaneger
        static object_pool<T> *poolManager;
        static bool usePool;
            //reference count to determinate when delete the object
        uint32_t  referenceCount;
        
        static AllocDef    allocPtr;
            // DeallocDef  deallocPtr;
        void (CPoolMemoryObject::*deallocPtr)();

        static T* _allocNew() {
            T *result = new T();
            result->deallocPtr = &CPoolMemoryObject::_deallocDelete;
            return result;
        }
        
        static T* _allocPool() {
            T *result = poolManager->construct();
            result->deallocPtr = &CPoolMemoryObject::_deallocDestroy;
            return result;
        }
        
        void _deallocDelete() {
            return delete(this);
        }
        
        void _deallocDestroy() {
            poolManager->destroy((T*)this);
        }
    public:
        
        /*
         Default constructor used for initialize the reference count
         */
        CPoolMemoryObject(){
            referenceCount = 1;
        }
        
        /*
         Set a neww pool mamaner, deleting the old one. Be aware
         deleting the ol pool, all object in use are deleted
         */
        static void setPoolManager(object_pool<T> *newPoolManager){
            poolManager = newPoolManager;
            if(poolManager){
                allocPtr = &CPoolMemoryObject::_allocPool;
            } else {
                allocPtr = &CPoolMemoryObject::_allocNew;
            }
        }
        
        /*
         Allocate and initialize a new object calling the constructor
         */
        static T* alloc(){
            return (*allocPtr)();
        }
        
        /*
         decrement the reference count and if it's = 0 delete the object
         */
        void release() {
            if(interprocess::ipcdetail::atomic_dec32(&referenceCount)==1){
                (this->*deallocPtr)();
            }
        }
        
        /*
         Increment the reference count
         */
        void retain () {
            interprocess::ipcdetail::atomic_inc32(&referenceCount);
        }
    };
    
    template <typename T> 
    typename CPoolMemoryObject<T>::AllocDef    CPoolMemoryObject<T>::allocPtr = &CPoolMemoryObject::_allocNew;

    
    template <typename T> 
    bool CPoolMemoryObject<T>::usePool=false;
    
        //Static initialization for the pool manager
    template <typename T> 
    object_pool<T>*CPoolMemoryObject<T>::poolManager;
}
#endif
