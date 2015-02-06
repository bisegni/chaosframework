/*
 *	MongoDBUnitServerDataAccess.cpp
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
#include "MongoDBUnitServerDataAccess.h"
#include <chaos/common/utility/TimingUtil.h>
#include "mongo_db_constants.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service::persistence::mongodb;

//DEFINE_LOGS_VARIABLE(MDBPDA, MongoDBUnitServerDataAccess)

#define MDBUSDA_INFO INFO_LOG(MongoDBUnitServerDataAccess)
#define MDBUSDA_DBG  DBG_LOG(MongoDBUnitServerDataAccess)
#define MDBUSDA_ERR  ERR_LOG(MongoDBUnitServerDataAccess)

MongoDBUnitServerDataAccess::MongoDBUnitServerDataAccess(const boost::shared_ptr<MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){
	
}

MongoDBUnitServerDataAccess::~MongoDBUnitServerDataAccess() {
    
}

//! insert the unit server information
int MongoDBUnitServerDataAccess::insertNewUS(chaos::common::data::CDataWrapper& unit_server_description) {
    CHAOS_ASSERT(node_data_access)
    //checn if the nedded field are present on datapack
    //if(unit_server_description)
    
    //cal node data access for insert abstract node
    return node_data_access->insertNewNode(unit_server_description);
}

int MongoDBUnitServerDataAccess::checkUSPresence(const std::string& unit_server_unique_id,
                                                 bool& presence) {
    CHAOS_ASSERT(node_data_access)
    return node_data_access->checkNodePresence(unit_server_unique_id,
                                               presence);
}

//! update the unit server information
/*!
 \param unit_server_description unit server key,value description
 */
int MongoDBUnitServerDataAccess::updateUS(chaos::common::data::CDataWrapper& unit_server_description) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObjBuilder updated_field;
    mongo::BSONObjBuilder bson_update;
    try {
        //serach criteria
        bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << unit_server_description.getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
        //update field
        updated_field << NodeDefinitionKey::NODE_RPC_ADDR << unit_server_description.getStringValue(NodeDefinitionKey::NODE_RPC_ADDR)
        << "ts" << mongo::Date_t(chaos::common::utility::TimingUtil::getTimeStamp());
        //get the contained control unit type
        mongo::BSONArrayBuilder bab;
        auto_ptr<CMultiTypeDataArrayWrapper> cu_type_array(unit_server_description.getVectorValue(UnitServerNodeDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_CONTROL_UNIT_ALIAS));
        for(int idx = 0;
            idx < cu_type_array->size();
            idx++) {
            bab.append(cu_type_array->getStringElementAtIndex(idx));
        }
        
        updated_field.appendArray(UnitServerNodeDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_CONTROL_UNIT_ALIAS, bab.arr());
        
        mongo::BSONObj query = bson_find.obj();
        
        //set the update
        bson_update << "$set" << updated_field.obj();
        mongo::BSONObj update = bson_update.obj();
        DEBUG_CODE(MDBUSDA_DBG << "updateUS update ---------------------------------------------";)
        DEBUG_CODE(MDBUSDA_DBG << "Query: "  << query.jsonString();)
        DEBUG_CODE(MDBUSDA_DBG << "Update: "  << update.jsonString();)
        DEBUG_CODE(MDBUSDA_DBG << "updateUS update ---------------------------------------------";)
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                     query,
                                     update))) {
            MDBUSDA_ERR << "Error updating unit server";
        }
    } catch (const mongo::DBException &e) {
        MDBUSDA_ERR << e.what();
        err = -1;
    }
    return err;
}


//! delete a unit server
int MongoDBUnitServerDataAccess::deleteUS(const std::string& unit_server_unique_id) {
    CHAOS_ASSERT(node_data_access)
    return node_data_access->deleteNode(unit_server_unique_id);
}