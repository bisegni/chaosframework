/*
 *	MongoDBProducerDataAccess.cpp
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
#include "MongoDBProducerDataAccess.h"

using namespace chaos::metadata_service::persistence::mongodb;

MongoDBProducerDataAccess::MongoDBProducerDataAccess(boost::shared_ptr<MongoDBHAConnectionManager>& _mongodb_connection):
mongodb_connection(_mongodb_connection) {
	
}

MongoDBProducerDataAccess::~MongoDBProducerDataAccess() {
	
}

int MongoDBProducerDataAccess::saveDataset(chaos::common::data::CDataWrapper& dataset_to_register) {
	return 0;
}

int MongoDBProducerDataAccess::loadLastDataset(const std::string& producer_name,
											   chaos::common::data::CDataWrapper **dataset_to_register) {
	return 0;
}