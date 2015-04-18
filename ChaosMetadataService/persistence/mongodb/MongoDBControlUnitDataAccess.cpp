/*
 *	MongoDBControlUnitDataAccess.cpp
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
#include "mongo_db_constants.h"
#include "MongoDBControlUnitDataAccess.h"

#include <chaos/common/utility/TimingUtil.h>

#include <mongo/client/dbclient.h>

#define MDBCUDA_INFO INFO_LOG(MongoDBControlUnitDataAccess)
#define MDBCUDA_DBG  DBG_LOG(MongoDBControlUnitDataAccess)
#define MDBCUDA_ERR  ERR_LOG(MongoDBControlUnitDataAccess)

using namespace chaos::common::data;

using namespace chaos::service_common::persistence::mongodb;

using namespace chaos::metadata_service::persistence::mongodb;

MongoDBControlUnitDataAccess::MongoDBControlUnitDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){

}

MongoDBControlUnitDataAccess::~MongoDBControlUnitDataAccess() {

}

int MongoDBControlUnitDataAccess::insertNewControlUnit(chaos::common::data::CDataWrapper& control_unit_description) {
    int err = 0;
    if(!control_unit_description.hasKey(chaos::NodeDefinitionKey::NODE_TYPE)) {
            //set he ndoe type as control unit
        control_unit_description.addStringValue(chaos::NodeDefinitionKey::NODE_TYPE, chaos::NodeType::NODE_TYPE_CONTROL_UNIT);
    }
    if((err = node_data_access->insertNewNode(control_unit_description))) {
        MDBCUDA_ERR << "Error:" << err << " adding new node for control unit";
    }
    return err;
}

int MongoDBControlUnitDataAccess::updateControlUnit(chaos::common::data::CDataWrapper& control_unit_description) {
    int err = 0;
    return err;
}

int MongoDBControlUnitDataAccess::addNewDataset(chaos::common::data::CDataWrapper& dataset_description) {
    int err = 0;
    return err;
}


int MongoDBControlUnitDataAccess::checkDatasetPresence(chaos::common::data::CDataWrapper& dataset_description)  {
    int err = 0;
    return err;
}


int MongoDBControlUnitDataAccess::getLastDataset(chaos::common::data::CDataWrapper& dataset_description)  {
    int err = 0;
    return err;
}

int MongoDBControlUnitDataAccess::setInstanceDescription(const std::string& cu_unique_id,
                                                         chaos::common::data::CDataWrapper& instance_description) {
    int err = 0;
        //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObjBuilder updated_field;
    mongo::BSONObjBuilder bson_update;
    MDBCUDA_DBG << instance_description.getJSONString();
    try {

        if(!instance_description.hasKey(chaos::NodeDefinitionKey::NODE_PARENT)) return -1;

            //serach criteria
        bson_find   << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << cu_unique_id
        << chaos::NodeDefinitionKey::NODE_TYPE << chaos::NodeType::NODE_TYPE_CONTROL_UNIT;

            //add the load_at_startup field
        updated_field << chaos::NodeDefinitionKey::NODE_PARENT << instance_description.getStringValue(chaos::NodeDefinitionKey::NODE_PARENT)
        << "auto_load" << (bool)(instance_description.hasKey("auto_load")?instance_description.getBoolValue("auto_load"):false);

            //add unit server parent

        if(instance_description.hasKey("load_parameter")) {
            updated_field << "load_parameter" << instance_description.getStringValue("load_parameter");
        }

        if(instance_description.hasKey("control_unit_implementation")) {
            updated_field << "control_unit_implementation" << instance_description.getStringValue("control_unit_implementation");
        }

            //check if have the driver description
        if(instance_description.hasKey("driver_description")) {
                //get the contained control unit type
            mongo::BSONArrayBuilder bab;
            auto_ptr<CMultiTypeDataArrayWrapper> drv_array(instance_description.getVectorValue("driver_description"));
            for(int idx = 0;
                idx < drv_array->size();
                idx++) {
                auto_ptr<CDataWrapper> driver_desc(drv_array->getCDataWrapperElementAtIndex(idx));
                if(driver_desc->hasKey("name") &&
                   driver_desc->hasKey("version")&&
                   driver_desc->hasKey("init_parameter")) {
                    int size;
                    CDataWrapper d;
                    d.addStringValue("name", driver_desc->getStringValue("name"));
                    d.addStringValue("version", driver_desc->getStringValue("version"));
                    d.addStringValue("init_parameter", driver_desc->getStringValue("init_parameter"));
                    bab << mongo::BSONObj(d.getBSONRawData(size));
                }

            }
                //bind the array to the key
            updated_field.appendArray("driver_description", bab.arr());
        }

            //check if we have the attribute setup
        if(instance_description.hasKey("attribute_value_descriptions")) {
                //get the contained control unit type
            mongo::BSONArrayBuilder bab;
            auto_ptr<CMultiTypeDataArrayWrapper> attr_array(instance_description.getVectorValue("attribute_value_descriptions"));
            for(int idx = 0;
                idx < attr_array->size();
                idx++) {
                auto_ptr<CDataWrapper> attr_desc(attr_array->getCDataWrapperElementAtIndex(idx));
                if(attr_desc->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME) &&
                   attr_desc->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE)) {
                    int size;
                    CDataWrapper a;
                    a.addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, attr_desc->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME));
                    a.addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE, attr_desc->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE));
                    if(attr_desc->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) a.addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE, attr_desc->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
                    if(attr_desc->hasKey(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) a.addStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE, attr_desc->getStringValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE));
                        //add object to array
                    bab << mongo::BSONObj(a.getBSONRawData(size));
                }
            }
                //bind the array to the key
            updated_field.appendArray("attribute_value_descriptions", bab.arr());
        }


        mongo::BSONObj query = bson_find.obj();
        mongo::BSONObj update = BSON("$set" << BSON("instance_description" << updated_field.obj()));

        DEBUG_CODE(MDBCUDA_DBG<<log_message("getInstanceDescription",
                                            "findOne",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    query.toString(),
                                                                    update.jsonString()));)
            //set the instance parameter within the node representing the control unit
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                     query,
                                     update))) {
            MDBCUDA_ERR << "Error updating unit server";
        }
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBControlUnitDataAccess::searchInstanceForUnitServer(std::vector<boost::shared_ptr<chaos::common::data::CDataWrapper> >& result_page,
                                                              const std::string& unit_server_uid,
                                                              std::vector<std::string> cu_type_filter,
                                                              uint32_t last_sequence_id,
                                                              uint32_t results_for_page) {
    int err = 0;
    mongo::BSONObjBuilder   bson_find;
    mongo::BSONArrayBuilder bson_find_or;
    mongo::BSONArrayBuilder bson_find_and;
    SearchResult            paged_result;

        //compose query

        //filter on sequence, type and unit server
    bson_find_and << BSON(chaos::NodeDefinitionKey::NODE_TYPE << chaos::NodeType::NODE_TYPE_CONTROL_UNIT);
    bson_find_and << BSON(boost::str(boost::format("instance_description.%1%") % chaos::NodeDefinitionKey::NODE_PARENT) << unit_server_uid);

        //add cu types
    if(cu_type_filter.size()) {
        for(std::vector<std::string>::iterator it = cu_type_filter.begin();
            it != cu_type_filter.end();
            it++) {
            bson_find_or << BSON("instance_description.control_unit_implementation" << *it);
        }
        bson_find_and << BSON("$or" << bson_find_or.arr());
    }
    bson_find.appendArray("$and", bson_find_and.obj());
        // filter on node unique id

    mongo::BSONObj q = bson_find.obj();
        // mongo::BSONObj p =  BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID<<1);
    DEBUG_CODE(MDBCUDA_DBG<<log_message("getInstanceDescription",
                                        "findOne",
                                        DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                q.toString()));)
        //perform the search for the query page
    if((err = performPagedQuery(paged_result,
                                MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                q,
                                NULL,//return only the control unit unique id
                                NULL,
                                results_for_page))) {
        MDBCUDA_ERR << "Error calling performPagedQuery with error" << err;
    } else {
        DEBUG_CODE(MDBCUDA_DBG << "The query '"<< q.toString() <<"' has found " << paged_result.size() << " result";)
        if(paged_result.size()) {
            for (SearchResultIterator it = paged_result.begin();
                 it != paged_result.end();
                 it++) {
                boost::shared_ptr<CDataWrapper> result_intance(new CDataWrapper());
                result_intance->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, it->getStringField(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));

                mongo::BSONObj instance_description = it->getObjectField("instance_description");
                result_intance->addStringValue("control_unit_implementation", instance_description.getStringField("control_unit_implementation"));

                result_page.push_back(result_intance);
            }
        }
    }
    return err;
}

int MongoDBControlUnitDataAccess::getInstanceDescription(const std::string& control_unit_uid,
                                                         chaos::common::data::CDataWrapper **result) {
    return getInstanceDescription("", control_unit_uid, result);
}

int MongoDBControlUnitDataAccess::getInstanceDescription(const std::string& unit_server_uid,
                                                         const std::string& control_unit_uid,
                                                         chaos::common::data::CDataWrapper **result) {
    int err = 0;
    mongo::BSONObj          q_result;
    mongo::BSONObjBuilder   bson_find;
    SearchResult            paged_result;

    bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << control_unit_uid;
    if(unit_server_uid.size()>0) {
            //ad also unit server for search the instance description
        bson_find << boost::str(boost::format("instance_description.%1%") % chaos::NodeDefinitionKey::NODE_PARENT) << unit_server_uid;
    }
    mongo::BSONObj q = bson_find.obj();

    DEBUG_CODE(MDBCUDA_DBG<<log_message("getInstanceDescription",
                           "findOne",
                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                   q.toString()));)

    if((err = connection->findOne(q_result,
                                  MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                  q))){
        MDBCUDA_ERR << "Error calling performPagedQuery with error" << err;
    } else if(q_result.isEmpty()){
        MDBCUDA_DBG << "No instance description found for control unit:" <<control_unit_uid << " with parent:" << unit_server_uid;
    } else {
        mongo::BSONObj instance_description = q_result.getObjectField("instance_description");
        *result = new CDataWrapper();
        (*result)->addStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID, q_result.getStringField(chaos::NodeDefinitionKey::NODE_UNIQUE_ID));


        (*result)->addStringValue(chaos::NodeDefinitionKey::NODE_PARENT, instance_description.getStringField(chaos::NodeDefinitionKey::NODE_PARENT));
        if(instance_description.hasField("auto_load"))(*result)->addBoolValue("auto_load", instance_description.getBoolField("auto_load"));
        if(instance_description.hasField("load_parameter"))(*result)->addStringValue("load_parameter", instance_description.getStringField("load_parameter"));
        if(instance_description.hasField("control_unit_implementation"))(*result)->addStringValue("control_unit_implementation", instance_description.getStringField("control_unit_implementation"));

        if(instance_description.hasField("driver_description")) {
            std::vector< mongo::BSONElement > drv_descriptions;
            instance_description.getObjectField("driver_description").elems(drv_descriptions);
            for(std::vector< mongo::BSONElement >::iterator it = drv_descriptions.begin();
                it != drv_descriptions.end();
                it++) {
                CDataWrapper driver_desc(it->Obj().objdata());
                (*result)->appendCDataWrapperToArray(driver_desc);
            }
            (*result)->finalizeArrayForKey("driver_description");
        }

        if(instance_description.hasField("attribute_value_descriptions")) {
            std::vector< mongo::BSONElement > attr_descriptions;
            instance_description.getObjectField("attribute_value_descriptions").elems(attr_descriptions);
            for(std::vector< mongo::BSONElement >::iterator it = attr_descriptions.begin();
                it != attr_descriptions.end();
                it++) {
                CDataWrapper driver_desc(it->Obj().objdata());
                (*result)->appendCDataWrapperToArray(driver_desc);
            }
            (*result)->finalizeArrayForKey("attribute_value_descriptions");
        }
    }
    return err;
}

int MongoDBControlUnitDataAccess::deleteInstanceDescription(const std::string& unit_server_uid,
                                                            const std::string& control_unit_uid) {
    int err = 0;
    mongo::BSONObj          q_result;
    mongo::BSONObjBuilder   bson_find;

    bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << control_unit_uid;
    bson_find << boost::str(boost::format("instance_description.%1%") % chaos::NodeDefinitionKey::NODE_PARENT) << unit_server_uid;
    mongo::BSONObj q = bson_find.obj();
    mongo::BSONObj u = BSON("$unset" << BSON("instance_description" << ""));

    DEBUG_CODE(MDBCUDA_DBG<<log_message("deleteInstanceDescription",
                           "update",
                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                   "Update",
                                                   q.toString(),
                                                   u.jsonString()));)
        //remove the field of the document
    if((err = connection->update(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                 q,
                                 u))) {
        MDBCUDA_ERR << "Error removing control unit instance from node";
    }
    return err;
}