/*
 *	MongoDBAgentDataAccess.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/02/2017 INFN, National Institute of Nuclear Physics
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

#include "MongoDBAgentDataAccess.h"
#include "mongo_db_constants.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/TimingUtil.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::service_common::data::node;
using namespace chaos::service_common::data::script;
using namespace chaos::service_common::data::dataset;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;
using namespace chaos::metadata_service::persistence::data_access;

#define INFO    INFO_LOG(MongoDBScriptDataAccess)
#define ERR     DBG_LOG(MongoDBScriptDataAccess)
#define DBG     ERR_LOG(MongoDBScriptDataAccess)

MongoDBAgentDataAccess::MongoDBAgentDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
AgentDataAccess(),
utility_data_access(NULL){}

MongoDBAgentDataAccess::~MongoDBAgentDataAccess() {}


int MongoDBAgentDataAccess::insertUpdateAgentDescription(CDataWrapper& agent_description) {
    int err = 0;
    int size = 0;
    try {
        CHECK_KEY_THROW_AND_LOG((&agent_description), NodeDefinitionKey::NODE_UNIQUE_ID, ERR, -1, CHAOS_FORMAT("The attribute %1% is not found", %NodeDefinitionKey::NODE_UNIQUE_ID));
        CHECK_KEY_THROW_AND_LOG((&agent_description), AgentNodeDefinitionKey::HOSTED_WORKER, ERR, -1, CHAOS_FORMAT("The attribute %1% is not found", %AgentNodeDefinitionKey::HOSTED_WORKER));
        //now update proprietary fields
        const std::string agent_uid = agent_description.getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << agent_uid
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_AGENT);
        mongo::BSONArrayBuilder array_descirption_builder;
        std::auto_ptr<CMultiTypeDataArrayWrapper> description_array(agent_description.getVectorValue(AgentNodeDefinitionKey::HOSTED_WORKER));
        for(int idx = 0;
            idx < description_array->size();
            idx++) {
            std::auto_ptr<CDataWrapper> worker_desc(description_array->getCDataWrapperElementAtIndex(idx));
            array_descirption_builder << mongo::BSONObj(worker_desc->getBSONRawData(size));
        }
        
        mongo::BSONObj update = BSON("$set" << BSON(AgentNodeDefinitionKey::HOSTED_WORKER << array_descirption_builder.arr()));
        
        DEBUG_CODE(DBG<<log_message("insertUpdateAgentDescription",
                                    "update",
                                    DATA_ACCESS_LOG_2_ENTRY("Query",
                                                            "Update",
                                                            query.toString(),
                                                            update.toString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     update,
                                     true,
                                     false))){
            ERR << "Error registering agent" << agent_uid << " with error:" << err;
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        ERR << e.what();
        err = e.errorCode;
    }
    return err;
}
