/*	
 *	DeviceDatasetCache.cpp
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

#include <cstdlib>
#include <fstream>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/upgradable_lock.hpp>

#include "DeviceDatasetCache.h"
#include "../../common/global.h"


using namespace chaos;
using namespace chaos::ui;
using namespace boost::interprocess;

DeviceDatasetCache::DeviceDatasetCache() {
    datasetSharedMemory = NULL;
    headerDatasetSharedMemory = NULL;
    
}

DeviceDatasetCache::~DeviceDatasetCache() {
    if(contantionHeader){
        DELETE_OBJ_POINTER(contantionHeader)
    }

    if(datasetHeader){
        DELETE_OBJ_POINTER(datasetHeader)
    }
    
    if(datasetSharedMemory){
        DELETE_OBJ_POINTER(datasetSharedMemory)
        shared_memory_object::remove(DATASET_SHARED_MEMORY);
    }
    
    if(headerDatasetSharedMemory){
        DELETE_OBJ_POINTER(headerDatasetSharedMemory)
        shared_memory_object::remove(INSTANCE_CONTENTION_SHARED_MEMORY);
    }
}


/*
 * Initzialize the datamanager
 */
void DeviceDatasetCache::init() throw(CException) {
    headerDatasetSharedMemory = new shared_memory_object(open_or_create,INSTANCE_CONTENTION_SHARED_MEMORY, read_write);
    headerDatasetSharedMemory->truncate(sizeof(ContantionCacheHeader));
    contentionHeaderRegion = new mapped_region( *headerDatasetSharedMemory, read_write, 0, sizeof(ContantionCacheHeader));

    contantionHeader = static_cast<ContantionCacheHeader*>(contentionHeaderRegion->get_address());

        //lock the header to check if all hase been initialized
    scoped_lock<interprocess_mutex> lock(contantionHeader->mutex);
    
    if(contantionHeader->initialized){
        datasetSharedMemory = new shared_memory_object(open_or_create,DATASET_SHARED_MEMORY, read_write);
        datasetHeaderRegion = new mapped_region( *datasetSharedMemory, read_write, 0, sizeof(DatasetCacheHeader));
        return;
    } else {
            //initialize the cache
        contantionHeader->initialized = true;

            //create the shared memory for the dataset shared  memory
        datasetSharedMemory = new shared_memory_object(open_or_create,DATASET_SHARED_MEMORY, read_write);
        datasetSharedMemory->truncate(sizeof(DatasetCacheHeader));
        datasetHeaderRegion = new mapped_region( *datasetSharedMemory, read_write, 0, sizeof(DatasetCacheHeader));
        datasetHeader =  static_cast<DatasetCacheHeader*>(datasetHeaderRegion->get_address());
        datasetHeader->currentOffset = sizeof(DatasetCacheHeader);
        
            //clean the array of the offset
        memset(datasetHeader->offsetArray, 0, sizeof(int32_t)*MAX_CACHE_ELEMENT);
    }
    
    lock.unlock();
}

/*
 * Deinitzialize the datamanager
 */
void DeviceDatasetCache::deinit() throw(CException) {

}

/*
 * Start all sub process
 */
void DeviceDatasetCache::start() throw(CException) {
    
}

/*
 Inizialize the shared memory cached
 */
void DeviceDatasetCache::grow(size_t segmentSize) throw(CException) {
    CHAOS_ASSERT(datasetHeader)
        //lock the header
    scoped_lock<interprocess_mutex> lock(datasetHeader->mutex);
    
    datasetSharedMemory->truncate(datasetHeader->currentOffset + segmentSize);
    datasetHeader->currentOffset += segmentSize;
}

/*
 return the mapped memory associated to the element
 of index idx, if this element don't fit the shared memory
 will be exapnded
 */
mapped_region *DeviceDatasetCache::getMappedRegionForIdx(std::size_t elementIdx, std::size_t elementDimension) {
    scoped_lock<interprocess_mutex> lock(datasetHeader->mutex);
    if(datasetHeader->offsetArray[elementIdx][0]==0){
            //enlarge the shared memory
        grow(elementDimension);
        datasetHeader->offsetArray[elementIdx][0]= datasetHeader->currentOffset;
        datasetHeader->offsetArray[elementIdx][1]= elementDimension;
        datasetHeader->currentOffset += elementDimension;
    }
        //return the mapping memory object
    return new mapped_region( *datasetSharedMemory, read_write, datasetHeader->offsetArray[elementIdx][0], datasetHeader->offsetArray[elementIdx][1]);
}
