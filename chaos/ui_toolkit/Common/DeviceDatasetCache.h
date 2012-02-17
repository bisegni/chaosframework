/*	
 *	DeviceDatasetCache.h
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

#ifndef ChaosFramework_DeviceDatasetCache_h
#define ChaosFramework_DeviceDatasetCache_h

#include <chaos/ui_toolkit/Common/CachedElement.h>
#include <chaos/common/general/Manager.h>


#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include <functional>
#include <utility>

#define INSTANCE_CONTENTION_SHARED_MEMORY "ic_shared_memory" 
#define DATASET_SHARED_MEMORY "ds_shared_memory"
#define MAX_CACHE_ELEMENT 32000
#define CACHE_EXTENT_DIMENSION 128000
namespace chaos {
    namespace ui {
        using namespace boost::interprocess;  
        /*
         This class represent the ahsred process header, for
         the dataset cache
         */
        struct ContantionCacheHeader {
                //flag for interprocess initialization managment
            bool initialized;
                //shared process mutext
            interprocess_mutex mutex;
        };
        
        /*
         This class represent the ahsred process header, for
         the dataset cache
         */
        struct DatasetCacheHeader {
                //flag for interprocess initialization managment
            bool initialized;
                //shared process mutext
            interprocess_mutex mutex;
                //current dimention
            std::size_t currentOffset;
            
            std::size_t offsetArray[MAX_CACHE_ELEMENT][2];//0-offset,1-length
        };        
        
        /*
         Chaching object for device dataset
         */
        class DeviceDatasetCache: public Manager {
                //cached header
            ContantionCacheHeader *contantionHeader;
            DatasetCacheHeader *datasetHeader;
            
                //shared memory mapped for the header
            mapped_region *contentionHeaderRegion;
            mapped_region *datasetHeaderRegion;
                //create the file lock
                //point to object for create shared memory
            shared_memory_object *headerDatasetSharedMemory;
            shared_memory_object *datasetSharedMemory;
                /*
                 expand the dataset shared memory
                 */
            void grow(size_t) throw(CException);
            
            /*
             return the mapped memory associated to the element
             of index idx, if this element don't fit the shared memory
             will be exapnded
             */
            mapped_region *getMappedRegionForIdx(std::size_t elementIdx, std::size_t elementDimension=0);
            
        public:
            DeviceDatasetCache();
            ~DeviceDatasetCache();
            /*
             * Initzialize the datamanager
             */
            void init() throw(CException);
            
            /*
             * Deinitzialize the datamanager
             */
            void deinit() throw(CException);
            
            /*
             * Start all sub process
             */
            void start() throw(CException);
            
        };
    }
}
#endif
