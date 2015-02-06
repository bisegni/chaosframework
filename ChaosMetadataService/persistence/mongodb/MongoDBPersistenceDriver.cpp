/*
 *	MongoDBPersistenceDriver.cpp
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
#include "MongoDBPersistenceDriver.h"
#include "MongoDBUnitServerDataAccess.h"
#include "MongoDBNodeDataAccess.h"

#include "../../mds_types.h"

using namespace chaos;
using namespace chaos::metadata_service::persistence;
using namespace chaos::metadata_service::persistence::mongodb;

DEFINE_CLASS_FACTORY(MongoDBPersistenceDriver, AbstractPersistenceDriver);

MongoDBPersistenceDriver::MongoDBPersistenceDriver(const std::string& name):
AbstractPersistenceDriver(name){
	
}
MongoDBPersistenceDriver::~MongoDBPersistenceDriver() {
	
}

void MongoDBPersistenceDriver::init(void *init_data) throw (chaos::CException) {
    //call sublcass
    AbstractPersistenceDriver::init(init_data);
    
	struct setting *_setting = static_cast<struct setting *>(init_data);
	if(!_setting) throw CException(-1, "No setting forwarded", __PRETTY_FUNCTION__);
	
	//we can configura the connection
	connection.reset(new MongoDBHAConnectionManager(_setting->persistence_server_list,
													_setting->persistence_kv_param_map));
    
    //register the data access implementations
    registerDataAccess<MongoDBUnitServerDataAccess, const boost::shared_ptr<MongoDBHAConnectionManager>& >(data_access_type::DataAccessTypeUnitServer,
                                                                                                         connection);
    registerDataAccess<MongoDBNodeDataAccess, const boost::shared_ptr<MongoDBHAConnectionManager>& >(data_access_type::DataAccessTypeNode,
                                                                                                           connection);
    
    //connec usda with nda
    ((MongoDBUnitServerDataAccess*) unit_server_da_instance)->node_data_access = (MongoDBNodeDataAccess*) node_da_instance;
}
void MongoDBPersistenceDriver::deinit() throw (chaos::CException) {
	connection.reset();
    //call sublcass
    AbstractPersistenceDriver::deinit();

}
