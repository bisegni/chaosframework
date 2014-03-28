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

using namespace chaos::data_service::index_system;

IndexDriver::IndexDriver(std::string alias):NamedService(alias), setting(NULL) {
	
}

IndexDriver::~IndexDriver() {
	if(setting) delete (setting);
}

//! init
void IndexDriver::init(void *init_data) throw (chaos::CException) {
	
}

//!deinit
void IndexDriver::deinit() throw (chaos::CException) {
	
}