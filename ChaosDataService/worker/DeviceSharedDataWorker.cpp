//
//  DeviceDataWorker.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 06/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "DeviceSharedDataWorker.h"
#include <chaos/common//data/cache/FastHash.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

using namespace chaos::data_service::worker;
namespace chaos_vfs = chaos::data_service::vfs;

#define DeviceSharedDataWorker_LOG_HEAD "[DeviceSharedDataWorker] - "
#define DSDW_LAPP_ LAPP_ << DeviceSharedDataWorker_LOG_HEAD
#define DSDW_LDBG_ LDBG_ << DeviceSharedDataWorker_LOG_HEAD << __FUNCTION__ << " - "
#define DSDW_LERR_ LERR_ << DeviceSharedDataWorker_LOG_HEAD << __FUNCTION__ << " - "


//------------------------------------------------------------------------------------------------------------------------

DeviceSharedDataWorker::DeviceSharedDataWorker(std::string _cache_impl_name, vfs::VFSManager *_vfs_manager_instance):
VFSFileSlotHashTable(18),
cache_impl_name(_cache_impl_name),
vfs_manager_instance(_vfs_manager_instance) {
	
}

DeviceSharedDataWorker::~DeviceSharedDataWorker() {
	VFSFileSlotHashTable::clear();
}

void DeviceSharedDataWorker::init(void *init_data) throw (chaos::CException) {
	DataWorker::init(init_data);
	DSDW_LAPP_ << "allocating cache driver for every thread";
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		thread_cookie[idx] = chaos::ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name.c_str());
	}
}

void DeviceSharedDataWorker::deinit() throw (chaos::CException) {
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		cache_system::CacheDriver *tmp_ptr = reinterpret_cast<cache_system::CacheDriver*>(thread_cookie[idx]);
		delete(tmp_ptr);
	}
	std::memset(thread_cookie, 0, sizeof(void*)*settings.job_thread_number);
	
	//clear hashtable
	VFSFileSlotHashTable::clear();
	
	DataWorker::deinit();
}

void DeviceSharedDataWorker::clearHashTableElement(void *key, uint32_t key_len, VFSFileSlot *element) {
	if(!element) return;
	//clear element
	vfs_manager_instance->releaseFile(element->file_ptr);
	delete(element);
}

VFSFileSlot *DeviceSharedDataWorker::getFileForKey(const void *key, const uint32_t key_len) {
	//chec if we have already a file slot
	VFSFileSlot *vfs_slot = NULL;
	if(!VFSFileSlotHashTable::getElement(key, key_len, &vfs_slot)) {
		//we have found the file slot and return it
		return vfs_slot;
	} else {
		//allocate a new slot
		VFSFileSlot *new_slot = new VFSFileSlot();
		new_slot->file_ptr = NULL;
		std::string path("data/");
		path.append((const char *)key, key_len);
		if(vfs_manager_instance->getFile(path, &new_slot->file_ptr)) {
			//we have got an error
			DSDW_LERR_ << "Error getting vfs file for " << path;
			return NULL;
		}
		
		//add slot to hash chain
		if(VFSFileSlotHashTable::addElement(key, key_len, new_slot)) {
			DSDW_LERR_ << "Error to add the vfs slot to hastable";
			vfs_manager_instance->releaseFile(new_slot->file_ptr);
			delete(new_slot);
			return NULL;
		}
		
		//al is gone well
		return new_slot;
	}
	
	//we can't go here...
}

void DeviceSharedDataWorker::writeHistoryData(DeviceSharedWorkerJob *job_ptr) {
	//
	VFSFileSlot *file_slot = getFileForKey(GET_PUT_OPCODE_KEY_PTR(job_ptr->request_header), job_ptr->request_header->key_len);
	if(!file_slot) {
		DSDW_LERR_ << "Error getting vfs file for hash " << job_ptr->request_header->device_hash;
	}
	
	//lock the vfs slot for protect it from other thread
	boost::unique_lock<boost::shared_mutex> lock_vfs_file(file_slot->mutex_slot);
	
	//write data
	file_slot->file_ptr->write(job_ptr->data_pack, job_ptr->data_pack_len);
}

void DeviceSharedDataWorker::executeJob(WorkerJobPtr job_info, void* cookie) {
	DeviceSharedWorkerJob *job_ptr = reinterpret_cast<DeviceSharedWorkerJob*>(job_info);
	//check what kind of push we have
	
	switch(job_ptr->request_header->tag) {
		case 0:{// storicize only
			//
			writeHistoryData(job_ptr);
			free(job_ptr->request_header);
			free(job_ptr->data_pack);
			free(job_info);
			break;
		}
			
		case 2:{// storicize and live
			writeHistoryData(job_ptr);
		}
			
		case 1:{// live only only
			cache_system::CacheDriver *cache_driver_instance = reinterpret_cast<cache_system::CacheDriver*>(cookie);
			cache_driver_instance->putData(GET_PUT_OPCODE_KEY_PTR(job_ptr->request_header),
										   job_ptr->request_header->key_len,
										   job_ptr->data_pack,
										   job_ptr->data_pack_len);
			free(job_ptr->request_header);
			free(job_ptr->data_pack);
			free(job_info);
			break;
		}
	}
}


void DeviceSharedDataWorker::addServer(std::string server_description) {
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		cache_system::CacheDriver *tmp_ptr = reinterpret_cast<cache_system::CacheDriver*>(thread_cookie[idx]);
		if(tmp_ptr) tmp_ptr->addServer(server_description);
	}
	
}

void DeviceSharedDataWorker::updateServerConfiguration() {
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		cache_system::CacheDriver *tmp_ptr = reinterpret_cast<cache_system::CacheDriver*>(thread_cookie[idx]);
		if(tmp_ptr) tmp_ptr->updateConfig();
	}
}