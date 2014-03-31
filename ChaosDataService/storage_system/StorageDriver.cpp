/*
 *	StorageDriver.cpp
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

#include "StorageDriver.h"

namespace chaos_vfs = chaos::data_service::vfs;

using namespace chaos::data_service::storage_system;

StorageDriver::StorageDriver(std::string alias):NamedService(alias), setting(NULL) {
	
}


StorageDriver::~StorageDriver() {
	
}

//! init
void StorageDriver::init(void *init_data) throw (chaos::CException) {
	setting = static_cast<StorageDriverSetting*>(init_data);
	if(!setting) throw CException(-1, "No setting has been set", __PRETTY_FUNCTION__);
}

//! deinit
void StorageDriver::deinit() throw (chaos::CException) {
	if(setting) {
		delete(setting);
		setting = NULL;
	}
}

chaos_vfs::DataBlock *StorageDriver::getNewDataBlock(std::string path) {
	return new chaos_vfs::DataBlock(path);
}

void StorageDriver::disposeDataBlock(chaos_vfs::DataBlock *data_block) {
	if(data_block) delete(data_block);
}