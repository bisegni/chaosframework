/*
 *	AbstractApiGroup.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#include "AbstractApiGroup.h"


using namespace chaos::wan_proxy::api;



AbstractApiGroup::AbstractApiGroup(const std::string& name):
NamedService(name){}

//! default destructor
AbstractApiGroup::~AbstractApiGroup() {}

void AbstractApiGroup::removeAllApi() {
	//call hash map clear method
	clear();
}

//! remove by name
void AbstractApiGroup::removeApiByName(const std::string api_name) {
	// call hash map method for remove an element by name
	removeElement(api_name.c_str(), (uint32_t)api_name.size());
}

#pragma mark Private Method
void AbstractApiGroup::clearHashTableElement(const void *key,
											 uint32_t key_len,
											 AbstractApi *element) {
	//delete the api instance
	delete(element);
}