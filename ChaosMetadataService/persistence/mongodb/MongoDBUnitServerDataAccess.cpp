/*
 *	MongoDBUnitServerDataAccess.cpp
 *	!CHAOS
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

MongoDBUnitServerDataAccess::MongoDBUnitServerDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){

}

MongoDBUnitServerDataAccess::~MongoDBUnitServerDataAccess() {

}

    //! insert the unit server information
int MongoDBUnitServerDataAccess::insertNewUS(chaos::common::data::CDataWrapper& unit_server_description) {
    CHAOS_ASSERT(node_data_access)
        //check if the nedded field are present on data pack
    if(!unit_server_description.hasKey(chaos::UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS)) return -1;

        //we have all filed so we can call the node data access api
    return node_data_access->insertNewNode(unit_server_description);
}

    //! update the unit server information
/*!
 \param unit_server_description unit server key,value description
 */
int MongoDBUnitServerDataAccess::updateUS(chaos::common::data::CDataWrapper& unit_server_description) {
    int err = 0;
    CHAOS_ASSERT(node_data_access)
        //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObjBuilder updated_field;
    mongo::BSONObjBuilder bson_update;
    try {
            //check if the nedded field are present on data pack
        if(!unit_server_description.hasKey(chaos::UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS)) return -1;

            //serach criteria
        bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << unit_server_description.getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);

            //get the contained control unit type
        mongo::BSONArrayBuilder bab;
        auto_ptr<CMultiTypeDataArrayWrapper> cu_type_array(unit_server_description.getVectorValue(UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS));
        for(int idx = 0;
            idx < cu_type_array->size();
            idx++) {
            bab.append(cu_type_array->getStringElementAtIndex(idx));
        }

        updated_field.appendArray(UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS, bab.arr());

        mongo::BSONObj query = bson_find.obj();

            //set the update
        bson_update << "$set" << updated_field.obj();
        mongo::BSONObj update = bson_update.obj();

        DEBUG_CODE(MDBUSDA_DBG<<log_message("updateUS",
                                            "update",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    query.jsonString(),
                                                                    update.jsonString()));)

            //first update thenode part then the unit server
        if((err = node_data_access->updateNode(unit_server_description))) {
            MDBUSDA_ERR << "Error updating node information";
            return err;
        } else if((err = connection->update(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                            query,
                                            update))) {
            MDBUSDA_ERR << "Error updating unit server";
        }
    } catch (const mongo::DBException &e) {
        MDBUSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBUSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBUnitServerDataAccess::checkUSPresence(const std::string& unit_server_unique_id,
                                                 bool& presence) {
    CHAOS_ASSERT(node_data_access)
    return node_data_access->checkNodePresence(unit_server_unique_id,
                                               presence);
}

    //! delete a unit server
int MongoDBUnitServerDataAccess::deleteUS(const std::string& unit_server_unique_id) {
    CHAOS_ASSERT(node_data_access)
    return node_data_access->deleteNode(unit_server_unique_id);
}

int MongoDBUnitServerDataAccess::getDescription(const std::string& unit_server_uid,
                                                chaos::common::data::CDataWrapper **unit_server_description) {
    CHAOS_ASSERT(node_data_access)
    int err = 0;
    if((err = node_data_access->getNodeDescription(unit_server_uid,
                                                   unit_server_description))) {
        MDBUSDA_ERR << "Error fetching the base ndoe attribute with code:" << err;
    } else if(!*unit_server_description) {
         MDBUSDA_ERR << "No data basic node attribute found for unit server" << unit_server_uid;
        err = -1;
    }else{
            //fetch the other unit server attribute
        mongo::BSONObj q =  BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << unit_server_uid);
        mongo::BSONObj p =  BSON(chaos::UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS << 1);
        DEBUG_CODE(MDBUSDA_DBG<<log_message("getDescription",
                                            "findOne",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Projection",
                                                                    q.jsonString(),
                                                                    p.jsonString()));)
        mongo::BSONObj r;
        if((err = connection->findOne(r,
                                      MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                      q,
                                      &p))){
            MDBUSDA_ERR << "Error fetching the unit server ndoe specific attribute with code:" << err;
        }else if(!r.isEmpty()) {
            if(r.hasField(chaos::UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS)) {
                mongo::BSONElement cu_types_element = r.getField(chaos::UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS);
                std::vector<mongo::BSONElement> cu_types = cu_types_element.Array();
                for(std::vector<mongo::BSONElement>::iterator it = cu_types.begin();
                    it != cu_types.end();
                    it++) {
                    (*unit_server_description)->appendStringToArray(it->String());
                }
                (*unit_server_description)->finalizeArrayForKey(chaos::UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS);
            } else {
                MDBUSDA_DBG << "No specific attribute found for unit server" << unit_server_uid;
            }
        }
        
    }
    return err;
}