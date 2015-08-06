/*
 *	StorageDriverMetricCollector.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#include "StorageDriverMetricCollector.h"

#include <chaos/common/global.h>

using namespace chaos::data_service::vfs::storage_system;

StorageDriverMetricCollector::StorageDriverMetricCollector(StorageDriver *_wrapped_driver):
StorageDriver("StorageDriverMetricCollector"),
wrapped_driver(_wrapped_driver),
metric_io(_wrapped_driver->getName()){
    
}

StorageDriverMetricCollector::~StorageDriverMetricCollector() {
    CHK_AND_DELETE_OBJ_POINTER(wrapped_driver)
}

void StorageDriverMetricCollector::initDomain() throw (chaos::CException) {
    CHAOS_ASSERT(wrapped_driver)
    wrapped_driver->initDomain();
}

void StorageDriverMetricCollector::init(void *init_data) throw (chaos::CException) {
    CHAOS_ASSERT(wrapped_driver)
    InizializableService::initImplementation(wrapped_driver,
                                             init_data,
                                             wrapped_driver->getName(),
                                             __PRETTY_FUNCTION__);
    metric_io.startLogging();
}

void StorageDriverMetricCollector::deinit() throw (chaos::CException) {
    CHAOS_ASSERT(wrapped_driver)
    metric_io.stopLogging();
    InizializableService::deinitImplementation(wrapped_driver,
                                               wrapped_driver->getName(),
                                               __PRETTY_FUNCTION__);
}

const std::string StorageDriverMetricCollector::getStorageDomain() const {
   CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->getStorageDomain();
}

int StorageDriverMetricCollector::blockIsPresent(chaos_vfs::DataBlock *data_block, bool &presence) {
   CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->blockIsPresent(data_block, presence);
}

int StorageDriverMetricCollector::openBlock(chaos_vfs::DataBlock *data_block, unsigned int flags) {
    CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->openBlock(data_block, flags);
}

int StorageDriverMetricCollector::openBlock(const std::string& vfs_path, unsigned int flags, chaos_vfs::DataBlock **data_block) {
    CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->openBlock(vfs_path, flags, data_block);
}

int StorageDriverMetricCollector::closeBlock(chaos_vfs::DataBlock *data_block) {
    CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->closeBlock(data_block);
}

int StorageDriverMetricCollector::getBlockSize(chaos_vfs::DataBlock *data_block) {
    CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->getBlockSize(data_block);
}

int StorageDriverMetricCollector::write(chaos_vfs::DataBlock *data_block, void * data, uint32_t data_len) {
    CHAOS_ASSERT(wrapped_driver)
    int err = wrapped_driver->write(data_block, data, data_len);
    if(!err) {
        metric_io.updateWriteBandwith(data_len);
    }
    return err;
}


int StorageDriverMetricCollector::read(chaos_vfs::DataBlock *data_block, void * buffer, uint32_t buffer_len, uint32_t& readed_byte) {
   CHAOS_ASSERT(wrapped_driver)
    int err = wrapped_driver->read(data_block, buffer, buffer_len, readed_byte);
    if(!err) {
        metric_io.updateReadBandwith(readed_byte);
    }
    return err;
}


int StorageDriverMetricCollector::seekp(chaos_vfs::DataBlock *data_block, uint64_t offset, chaos_vfs::block_seek_base::BlockSeekBase base_direction) {
   CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->seekg(data_block, offset, base_direction);
}


int StorageDriverMetricCollector::seekg(chaos_vfs::DataBlock *data_block, uint64_t offset, chaos_vfs::block_seek_base::BlockSeekBase base_direction) {
    CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->seekg(data_block, offset, base_direction);
}


int StorageDriverMetricCollector::tellp(chaos_vfs::DataBlock *data_block, uint64_t& offset) {
    CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->tellp(data_block, offset);
}


int StorageDriverMetricCollector::tellg(chaos_vfs::DataBlock *data_block, uint64_t& offset) {
    CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->tellg(data_block, offset);
}


int StorageDriverMetricCollector::resize(chaos_vfs::DataBlock *data_block, uint64_t new_size) {
    CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->resize(data_block, new_size);
}


int StorageDriverMetricCollector::flush(chaos_vfs::DataBlock *data_block) {
    CHAOS_ASSERT(wrapped_driver)
    int err = wrapped_driver->flush(data_block);
    if(!err) {
        metric_io.incrementFlushCall();
    }
    return err;
}


int StorageDriverMetricCollector::createDirectory(const std::string& vfs_path) {
    CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->createDirectory(vfs_path);
}


int StorageDriverMetricCollector::createPath(const std::string& vfs_path) {
    CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->createPath(vfs_path);
}


int StorageDriverMetricCollector::deletePath(const std::string& vfs_path, bool all) {
    CHAOS_ASSERT(wrapped_driver)
    return wrapped_driver->deletePath(vfs_path, all);
}

