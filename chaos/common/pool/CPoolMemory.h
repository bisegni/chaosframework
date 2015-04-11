/*	
 *	CPoolMemory.h
 *	!CHAOS
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
#ifndef ChaosFramework_CPoolMemory_h
#define ChaosFramework_CPoolMemory_h
#include <stdint.h>
#include <queue>
#include <map>
#include <boost/thread/mutex.hpp>

namespace chaos {
    
    using namespace std;
    using namespace boost;
    
#define DEFAULT_POOL_OBJECT_NUMBER  100 // number of object in the pool
    
    /*
     This Class provide an effice pool mamaner implementation for Chaos Library purpose,
     it manage, for a determinate kind of object a block of contiguos memory, to remove
     Heap fragmentation into CHAOS Control Unit. CU create a lot of CDataWrapper objet
     and all of this are manage into differento output buffer in the same time. So an 
     efficent memory managing for this object is need
     */
    template <typename T>
    class CPoolMemory {
        
        /*
         Memory Allocation Record, is used to
         describe a chunk of memory accordind to object dimension
         this struct will be 4 bite larger then object size
         */
        typedef struct{
            int32_t location;      //4 char size
                                   //keep track of retain on the object
            int32_t memRetain;      //4 char size
            char objMem[sizeof(T)]; //char array for size of object
        } MemoryAllocationRecord, *MemoryAllocationRecordPtr;
        
        MemoryAllocationRecordPtr memoryPool;
        
        boost::mutex threadPoolAccessMutex;
        
        queue<int> unallocatedRecordIndex;
        
        map< intptr_t,  MemoryAllocationRecordPtr > usedRecordMap;
    public:
        CPoolMemory(long initialPoolSize = DEFAULT_POOL_OBJECT_NUMBER){
                //initizialize the buffer for a numebr of object
            memoryPool = new MemoryAllocationRecord[initialPoolSize];
            
                //clear all memory
            memset(memoryPool, 0, initialPoolSize * sizeof(MemoryAllocationRecord));
            
            int idx = 0;
            for (; idx < initialPoolSize; idx++) {
                unallocatedRecordIndex.push(idx);
            }
        }
        
        ~CPoolMemory(){
            if(memoryPool) delete [] memoryPool;
        }
        
        
        /*
         Allocate anew object form the pool
         */
        T* getObjectNewInstance() {
            boost::mutex::scoped_lock lock(threadPoolAccessMutex);
            T* returnAddress = NULL;
            
            if(!unallocatedRecordIndex.size()) return returnAddress;
            
            int locIdx = unallocatedRecordIndex.front();
                // to get new instance
            MemoryAllocationRecord *nextFreeRecord = memoryPool + locIdx;
            
                //remove the oldest data
            unallocatedRecordIndex.pop();
            
            nextFreeRecord->location = locIdx;
            nextFreeRecord->memRetain++;
            
                //insert used record into map
            returnAddress = reinterpret_cast<T*>(nextFreeRecord->objMem);
            usedRecordMap.insert(make_pair((intptr_t)returnAddress, nextFreeRecord));
            
            return returnAddress;
        }
        
        /*
         release an allocated space of memory for
         next reuse;
         */
        void release(T *pointer) {
            boost::mutex::scoped_lock lock(threadPoolAccessMutex);
            
                //check the record associated to the pointer
            if(!pointer || !usedRecordMap.count((intptr_t)pointer)) return;
            
                //found th ekey
            if(!(--usedRecordMap[(intptr_t)pointer]->memRetain)){
                    //call destructor
                pointer->~T();
                    //retrive the struct
                MemoryAllocationRecord *usedRecord = usedRecordMap[(intptr_t)pointer];
                
                unallocatedRecordIndex.push(usedRecord->location);
                
                    //reset and reuse the object
                memset(usedRecord, 0, sizeof(MemoryAllocationRecord));
            }
        }
        
        /*
         release an allocated space of memory for
         next reuse;
         */
        void retain(T *pointer) {
            boost::mutex::scoped_lock lock(threadPoolAccessMutex);
            
                //check the record associated to the pointer
            if(!pointer || !usedRecordMap.count((intptr_t)pointer)) return;
            
            usedRecordMap[(intptr_t)pointer]->memRetain++;
        }
    };
    
}
#endif
