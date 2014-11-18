/*
 *	StorageDriver.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

using namespace chaos::data_service::vfs::storage_system;

StorageDriver::StorageDriver(std::string alias):NamedService(alias), setting(NULL) {
	
}


StorageDriver::~StorageDriver() {
	
}

//! init
void StorageDriver::init(void *init_data) throw (chaos::CException) {
	setting = static_cast<StorageDriverSetting*>(init_data);
	if(!setting) throw CException(-1, "No setting set", __PRETTY_FUNCTION__);
	if(!setting->domain.name.size()) throw CException(-1, "No domain name set", __PRETTY_FUNCTION__);
	
	//initialize driver domain
	initDomain();
}

//! deinit
void StorageDriver::deinit() throw (chaos::CException) {
}

//! return the storage domain for this driver
const std::string StorageDriver::getStorageDomain() const {
	return setting->domain.name;
}