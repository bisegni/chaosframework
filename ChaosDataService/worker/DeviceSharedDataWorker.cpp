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
cache_impl_name(_cache_impl_name),
vfs_manager_instance(_vfs_manager_instance),
vfs_file_hash_hashpower(16),
vfs_file_hash_mask(hashmask(vfs_file_hash_hashpower)),
vfs_file_hash_slot(NULL) {
	
}

DeviceSharedDataWorker::~DeviceSharedDataWorker() {
}

void DeviceSharedDataWorker::init(void *init_data) throw (chaos::CException) {
	DataWorker::init(init_data);
	DSDW_LAPP_ << "allocating cache driver for every thread";
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		thread_cookie[idx] = chaos::ObjectFactoryRegister<cache_system::CacheDriver>::getInstance()->getNewInstanceByName(cache_impl_name.c_str());
	}
	
	DSDW_LAPP_ << "allocating cache driver for every thread";
	uint64_t hash_size = hashsize(vfs_file_hash_hashpower);
	vfs_file_hash_slot = (VFSFileSlot**)calloc(hash_size, sizeof(VFSFileSlot**));
	if(!vfs_file_hash_slot) throw chaos::CException(-1,"Error allocating memory for vfs files hash", __PRETTY_FUNCTION__);
	
	std::memset(vfs_file_hash_slot, 0, hash_size*sizeof(vfs::VFSFile**));
}

void DeviceSharedDataWorker::deinit() throw (chaos::CException) {
	if(vfs_file_hash_slot){
		//release all file
		for (int idx = 0; idx < hashsize(vfs_file_hash_hashpower); idx++) {
			if(vfs_file_hash_slot[idx]) {
				vfs_manager_instance->releaseFile(vfs_file_hash_slot[idx]->file_ptr);
				delete(vfs_file_hash_slot[idx]);
				vfs_file_hash_slot[idx] = NULL;
			}
		}
	}
	for(int idx = 0; idx < settings.job_thread_number; idx++) {
		cache_system::CacheDriver *tmp_ptr = reinterpret_cast<cache_system::CacheDriver*>(thread_cookie[idx]);
		delete(tmp_ptr);
	}
	std::memset(thread_cookie, 0, sizeof(void*)*settings.job_thread_number);
	DataWorker::deinit();
}

VFSFileSlot *DeviceSharedDataWorker::findSlot(const void * key, uint32_t key_len) {
	uint32_t hash_val = chaos::common::data::cache::FastHash::hash(key, key_len, 0);
    VFSFileSlot *slot = vfs_file_hash_slot[hash_val & vfs_file_hash_mask];
    VFSFileSlot *ret = NULL;
    while (slot) {
        if ((key_len == slot->key_len) && (memcmp(key, slot->key, key_len) == 0)) {
            ret = slot;
            break;
        }
        slot = slot->next;
    }
    return ret;
}

void DeviceSharedDataWorker::addSlot(const void *key, uint32_t key_len, VFSFileSlot *new_slot) {
    //we can't add two times the same key to hash
    assert(findSlot(key, key_len) == 0);
    uint32_t hash_val = chaos::common::data::cache::FastHash::hash(key, key_len, 0);
	
	//clean list pointer
	new_slot->prev = new_slot->next = NULL;
	
	//get last slot with same hash
	VFSFileSlot *prev_slot = vfs_file_hash_slot[hash_val & vfs_file_hash_mask];
	if(prev_slot) {
		prev_slot->prev = new_slot;
		new_slot->next = prev_slot;
	}
	vfs_file_hash_slot[hash_val & vfs_file_hash_mask] = new_slot;
}


void DeviceSharedDataWorker::removeSlot(const void *key, uint32_t key_len) {
	uint32_t hash_val = chaos::common::data::cache::FastHash::hash(key, key_len, 0);
    VFSFileSlot *slot = vfs_file_hash_slot[hash_val & vfs_file_hash_mask];
    while (slot) {
        if ((key_len == slot->key_len) && (memcmp(key, slot->key, key_len) == 0)) {
            if(slot->prev) {
				slot->next->prev = slot->prev;
			} else {
				//is the first of the list so set the next as first
				vfs_file_hash_slot[hash_val & vfs_file_hash_mask] = slot->next;
			}
            break;
		} else {
			slot = slot->next;
		}
    }
}

VFSFileSlot *DeviceSharedDataWorker::getFileForKey(const void *key, const uint32_t key_len) {
	//lock on read
	boost::upgrade_lock<shared_mutex> readLock(mutex_hash);
	VFSFileSlot *vfs_slot = findSlot(key, key_len);
	if(vfs_slot) {
		//we have found the file
		return vfs_slot;
	} else {
		//we need to add the new file
		
		//lock on write
		boost::upgrade_to_unique_lock<shared_mutex> writeLock(readLock);
		
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
		//copy the key
		new_slot->key = malloc(key_len);
		std::memcpy(new_slot->key, key, (new_slot->key_len = key_len));
		
		//add slot to hash chain
		addSlot(key, key_len, new_slot);
		return new_slot;
	}
	
	//we ca go here...
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