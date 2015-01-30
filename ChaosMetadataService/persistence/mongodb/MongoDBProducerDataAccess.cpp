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

//DEFINE_LOGS_VARIABLE(MDBPDA, MongoDBProducerDataAccess)

#define MDBPDA_INFO INFO_LOG(MongoDBProducerDataAccess)
#define MDBPDA_DBG  INFO_DBG(MongoDBProducerDataAccess)
#define MDBPDA_ERR  INFO_ERR(MongoDBProducerDataAccess)

MongoDBProducerDataAccess::MongoDBProducerDataAccess(const boost::shared_ptr<MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){
	
}

MongoDBProducerDataAccess::~MongoDBProducerDataAccess() {
	
}

// inherited method
int MongoDBProducerDataAccess::insertNewProducer(chaos::common::data::CDataWrapper& producer_property) {
    return 0;
}

// inherited method
int MongoDBProducerDataAccess::checkProducerPresence(const std::string& producer_unique_name, bool& found) {
    return 0;
}

// inherited method
int MongoDBProducerDataAccess::saveDataset(const std::string& producer_unique_name,
                                           chaos::common::data::CDataWrapper& dataset_to_register) {
    return 0;
}

// inherited method
int MongoDBProducerDataAccess::loadLastDataset(const std::string& producer_unique_name,
                                               chaos::common::data::CDataWrapper **dataset_to_load) {
    return 0;
}