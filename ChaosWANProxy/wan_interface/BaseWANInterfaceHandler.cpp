/*
 *	BaseWANInterfacelHandler.cpp
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
#include "BaseWANInterfaceHandler.h"

#include <chaos/common/global.h>

using namespace chaos::wan_proxy::api;
using namespace chaos::wan_proxy::wan_interface;

#pragma mark ApiGroupVersionDomain
ApiGroupVersionDomain::ApiGroupVersionDomain():
//set the hash map power to 8
ApiGroupHashMap(8){}

ApiGroupVersionDomain::~ApiGroupVersionDomain() {}

// inherited from AbstractApiHashMap
void ApiGroupVersionDomain::clearHashTableElement(const void *key,
												  uint32_t key_len,
												  api::AbstractApiGroup *element) {
	//delete the element
	delete(element);
}

int ApiGroupVersionDomain::callGroupApi(std::vector<std::string>& api_tokens,
										const Json::Value& input_data,
										std::map<std::string, std::string>& output_header,
										Json::Value& output_data) {
	int err = 0;
	//get the first tocken for the group name
	const std::string& group_name = api_tokens.front();
	AbstractApiGroup *api_group_selected = NULL;
	if((err = getElement(group_name.c_str(), (uint32_t)group_name.size(), &api_group_selected)) ||
	   !api_group_selected) {
		//error or not found
	} else {
		//remove the group name
		api_tokens.erase(api_tokens.begin());
		
		//forward call to the found group
		err=api_group_selected->callApi(api_tokens,
									input_data,
									output_header,
									output_data);
	}
	return err;
}
#pragma mark BaseWANInterfacelHandler

// default constructor
BaseWANInterfacelHandler::BaseWANInterfacelHandler(persistence::AbstractPersistenceDriver *_persistence_driver):
PersistenceAccessor(_persistence_driver) {
	//the element at position 0 is a fake to do a 1 based vector
	api_group_version_domain_list.push_back(NULL);
}

// default destructor
BaseWANInterfacelHandler::~BaseWANInterfacelHandler() {
    //delete all version
	for(GroupVersionListIterator it = api_group_version_domain_list.begin();
		it != api_group_version_domain_list.end();
		it++) {
		if(*it) delete(*it);
	}
	api_group_version_domain_list.clear();
}

// call the api
int BaseWANInterfacelHandler::handleCall(int version,
											 std::vector<std::string>& api_tokens,
											 const Json::Value& input_data,
											 std::map<std::string, std::string>& output_header,
											 Json::Value& output_data) {
	CHAOS_ASSERT(version)
	if(api_group_version_domain_list.size() <= version) return -1;
	return api_group_version_domain_list[version]->callGroupApi(api_tokens,
																input_data,
																output_header,
																output_data);
}
