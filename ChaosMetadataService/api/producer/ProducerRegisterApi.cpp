/*
 *	ProducerRegisterApi.cpp
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
#include "ProducerRegisterApi.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::producer;

ProducerRegisterApi::ProducerRegisterApi():
AbstractApi("register"){
	
}

ProducerRegisterApi::~ProducerRegisterApi() {
	
}

chaos::common::data::CDataWrapper *ProducerRegisterApi::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
	return NULL;
}