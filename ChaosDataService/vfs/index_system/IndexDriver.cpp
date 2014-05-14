/*
 *	IndexDriver.cpp
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

#include "IndexDriver.h"
#include "../DataBlock.h"

using namespace chaos::data_service::index_system;

IndexDriver::IndexDriver(std::string alias):NamedService(alias) {
	
}

IndexDriver::~IndexDriver() {
	
}

//! init
void IndexDriver::init(void *init_data) throw (chaos::CException) {
	setting = static_cast<IndexDriverSetting*>(init_data);
	if(!setting) throw CException(-1, "No setting set", __PRETTY_FUNCTION__);
	if(!setting->servers.size()) throw CException(-1, "No server set", __PRETTY_FUNCTION__);

}

//!deinit
void IndexDriver::deinit() throw (chaos::CException) {
}

chaos::data_service::chaos_vfs::DataBlock *IndexDriver::getEmptyDataBlock() {
	return new chaos::data_service::chaos_vfs::DataBlock();
}

void IndexDriver::deleteDataBlock(chaos::data_service::chaos_vfs::DataBlock *data_block) {
	delete(data_block);
}