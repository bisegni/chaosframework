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

#include <boost/format.hpp>
#include <boost/foreach.hpp>

#include <mongo/client/dbclient.h>

#define MDBCUDA_INFO INFO_LOG(MongoDBControlUnitDataAccess)
#define MDBCUDA_DBG  DBG_LOG(MongoDBControlUnitDataAccess)
#define MDBCUDA_ERR  ERR_LOG(MongoDBControlUnitDataAccess)

using namespace chaos;
using namespace chaos::common::data;

using namespace chaos::service_common::persistence::mongodb;

using namespace chaos::metadata_service::persistence::mongodb;

MongoDBControlUnitDataAccess::MongoDBControlUnitDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){
    
}

MongoDBControlUnitDataAccess::~MongoDBControlUnitDataAccess() {
    
}

int MongoDBControlUnitDataAccess::checkPresence(const std::string& unit_server_unique_id,
                                                bool& presence) {
    CHAOS_ASSERT(node_data_access)
    return node_data_access->checkNodePresence(presence,
                                               unit_server_unique_id,
                                               NodeType::NODE_TYPE_CONTROL_UNIT);
}

int MongoDBControlUnitDataAccess::getControlUnitWithAutoFlag(const std::string& unit_server_host,
                                                             chaos::metadata_service::persistence::AutoFlag auto_flag,
                                                             uint64_t last_sequence_id,
                                                             std::vector<NodeSearchIndex>& control_unit_found) {
    int err = 0;
    SearchResult paged_result;
    try {
        mongo::BSONObjBuilder query_builder;
        
        //filter on unit server
        query_builder << boost::str(boost::format("instance_description.%1%") % NodeDefinitionKey::NODE_PARENT) << unit_server_host;
        
        switch(auto_flag) {
            case AUTO_LOAD:
                query_builder << "instance_description.auto_load" << true;
                break;
            case AUTO_INIT:
                return 0;
                break;
            case AUTO_START:
                return 0;
                break;
        }
        
        query_builder << "seq" << BSON("$gte"<<(long long)last_sequence_id);
        mongo::Query query = query_builder.obj();
        
        DEBUG_CODE(MDBCUDA_DBG<<log_message("checkDatasetPresence",
                                            "performPagedQuery",
                                            DATA_ACCESS_LOG_1_ENTRY("query",
                                                                    query.toString()));)
        //remove the field of the document
        if((err = performPagedQuery(paged_result,
                                   MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                   query.sort("seq"),
                                   NULL,
                                   NULL,
                                   30))) {
            MDBCUDA_ERR << "Error performing auto flag search";
        } else {
            for(SearchResultIterator it = paged_result.begin();
                it != paged_result.end();
                it++) {
                control_unit_found.push_back(NodeSearchIndex(it->getField("seq").numberLong(),
                                                             it->getField(NodeDefinitionKey::NODE_UNIQUE_ID).String()));
            }
        }
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;

}

int MongoDBControlUnitDataAccess::insertNewControlUnit(CDataWrapper& control_unit_description) {
    int err = 0;
    if(!control_unit_description.hasKey(NodeDefinitionKey::NODE_TYPE)) {
        //set the type of control unit
        control_unit_description.addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_CONTROL_UNIT);
    }
    if((err = node_data_access->insertNewNode(control_unit_description))) {
        MDBCUDA_ERR << "Error:" << err << " adding new node for control unit";
    }
    return err;
}

int MongoDBControlUnitDataAccess::updateControlUnit(CDataWrapper& control_unit_description) {
    int err = 0;
    return err;
}

int MongoDBControlUnitDataAccess::setDataset(const std::string& cu_unique_id,
                                             CDataWrapper& dataset_description) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObjBuilder updated_field;
    mongo::BSONObjBuilder bson_update;
    try {
        //check for principal mandatory keys
        if(cu_unique_id.size() == 0) return -1;
        if(!dataset_description.hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION))return -2;
        if(!dataset_description.hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TIMESTAMP))return -3;
        
        //serach criteria
        bson_find   << NodeDefinitionKey::NODE_UNIQUE_ID << dataset_description.getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID)
        << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_CONTROL_UNIT;
        
        //add dataset timestamp
        updated_field << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TIMESTAMP << (long long)dataset_description.getInt64Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TIMESTAMP);
        
        //add dataset
        std::auto_ptr<CMultiTypeDataArrayWrapper> ds_vec(dataset_description.getVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION));
        if(!ds_vec.get()) return -3;
        //cicle all dataset attribute
        mongo::BSONArrayBuilder dataset_bson_array;
        int32_t type = 0;
        for(int idx = 0;
            idx < ds_vec->size();
            idx++) {
            mongo::BSONObjBuilder dataset_element_builder;
            
            auto_ptr<CDataWrapper> dataset_element(ds_vec->getCDataWrapperElementAtIndex(idx));
            if(dataset_element->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME) &&
               dataset_element->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION)&&
               dataset_element->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)&&
               dataset_element->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION)) {
                
                //mandatory field
                dataset_element_builder << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME <<
                dataset_element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME);
                
                dataset_element_builder << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION <<
                dataset_element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION);
                
                dataset_element_builder << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION <<
                dataset_element->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION);
                
                dataset_element_builder << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE <<
                (type = dataset_element->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE));
                
                if(type == DataType::TYPE_BYTEARRAY) {
                    //we need to have addtional information for binary data
                    if(dataset_element->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE)) {
                        
                        if(dataset_element->isVector(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE)) {
                            //we have multipler value for subtype
                            mongo::BSONArrayBuilder subtype_array_builder;
                            std::auto_ptr<CMultiTypeDataArrayWrapper> sub_t_vec(dataset_element->getVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE));
                            for(int idx = 0;
                                idx < sub_t_vec->size();
                                idx++) {
                                subtype_array_builder << sub_t_vec->getInt32ElementAtIndex(idx);
                            }
                            //add array to dataset element
                            dataset_element_builder << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE
                            << subtype_array_builder.arr();
                        } else {
                            //we have only one value
                            dataset_element_builder << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE
                            << dataset_element->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE);
                        }
                    }
                }
                
                if(dataset_element->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE)) {
                    dataset_element_builder << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE
                    << dataset_element->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE);
                }
                
                //optional field
                if(dataset_element->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE)) {
                    dataset_element_builder << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE <<
                    dataset_element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE);
                }
                
                if(dataset_element->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) {
                    dataset_element_builder << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE <<
                    dataset_element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE);
                }
                
                if(dataset_element->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) {
                    dataset_element_builder << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE <<
                    dataset_element->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE);
                }
                
                dataset_bson_array << dataset_element_builder.obj();
            }
        }
        //add dataset array to update bson
        updated_field.appendArray(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION,
                                  dataset_bson_array.arr());
        
        //check if we have bactch command in the dataset
        if(dataset_description.hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_COMMAND_DESCRIPTION)) {
            std::auto_ptr<CMultiTypeDataArrayWrapper> bc_vec(dataset_description.getVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_COMMAND_DESCRIPTION));
            mongo::BSONArrayBuilder batch_command_bson_array;
            for(int idx = 0;
                idx < bc_vec->size();
                idx++) {
                mongo::BSONObjBuilder batch_command_builder;
                
                auto_ptr<CDataWrapper> bc_element(bc_vec->getCDataWrapperElementAtIndex(idx));
                MDB_COPY_STRING_CDWKEY_TO_BUILDER(batch_command_builder, bc_element, common::batch_command::BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)
                MDB_COPY_STRING_CDWKEY_TO_BUILDER(batch_command_builder, bc_element, common::batch_command::BatchCommandAndParameterDescriptionkey::BC_ALIAS)
                MDB_COPY_STRING_CDWKEY_TO_BUILDER(batch_command_builder, bc_element, common::batch_command::BatchCommandAndParameterDescriptionkey::BC_DESCRIPTION)
                
                //check for parameter
                if(bc_element->hasKey(common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETERS)){
                    mongo::BSONArrayBuilder batch_command_parameter_bson_array;
                    std::auto_ptr<CMultiTypeDataArrayWrapper> bc_param_vec(bc_element->getVectorValue(common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETERS));
                    for(int idx_param = 0;
                        idx_param < bc_param_vec->size();
                        idx_param++) {
                        mongo::BSONObjBuilder batch_command_parameter_builder;
                        auto_ptr<CDataWrapper> bc_param_element(bc_param_vec->getCDataWrapperElementAtIndex(idx_param));
                        MDB_COPY_STRING_CDWKEY_TO_BUILDER(batch_command_parameter_builder, bc_param_element, common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME)
                        MDB_COPY_STRING_CDWKEY_TO_BUILDER(batch_command_parameter_builder, bc_param_element, common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION)
                        MDB_COPY_I32_CDWKEY_TO_BUILDER(batch_command_parameter_builder, bc_param_element, common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE)
                        MDB_COPY_I32_CDWKEY_TO_BUILDER(batch_command_parameter_builder, bc_param_element, common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG)
                        
                        batch_command_parameter_bson_array << batch_command_parameter_builder.obj();
                    }
                    //add dataset array to update bson
                    batch_command_builder.appendArray(common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETERS,
                                                      batch_command_parameter_bson_array.arr());
                }
                //add the batch command description to the command array
                batch_command_bson_array << batch_command_builder.obj();
                
                //insert commandi onto node comamnd collection
                node_data_access->setCommand(*bc_element);
            }
            
            //add batch command to the dataset
            updated_field.appendArray(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_COMMAND_DESCRIPTION,
                                      batch_command_bson_array.arr());
        }
        
        mongo::BSONObj query = bson_find.obj();
        mongo::BSONObj update = BSON("$set" << BSON(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION << updated_field.obj()));
        
        DEBUG_CODE(MDBCUDA_DBG<<log_message("getInstanceDescription",
                                            "findOne",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    query.toString(),
                                                                    update.jsonString()));)
        //set the instance parameter within the node representing the control unit
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     update))) {
            MDBCUDA_ERR << "Error updating unit server";
        }
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}


int MongoDBControlUnitDataAccess::checkDatasetPresence(const std::string& cu_unique_id,
                                                       bool& presence) {
    int err = 0;
    unsigned long long counter;
    try {
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << cu_unique_id
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_CONTROL_UNIT
                                    << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION  << BSON("$exists" << true ));
        
        
        DEBUG_CODE(MDBCUDA_DBG<<log_message("checkDatasetPresence",
                                            "find",
                                            DATA_ACCESS_LOG_1_ENTRY("query",
                                                                    query.toString()));)
        //remove the field of the document
        if((err = connection->count(counter,
                                    MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                    query))) {
            MDBCUDA_ERR << "Error counting the element";
        } else {
            presence = (counter == 1);
        }
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBControlUnitDataAccess::getDataset(const std::string& cu_unique_id,
                                             chaos::common::data::CDataWrapper **dataset_description) {
    int err = 0;
    mongo::BSONObj result;
    try {
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << cu_unique_id
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_CONTROL_UNIT
                                    << ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION  << BSON("$exists" << true ));
        mongo::BSONObj prj = BSON(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION  << 1 <<
                                  ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_COMMAND_DESCRIPTION << 1);
        
        
        DEBUG_CODE(MDBCUDA_DBG<<log_message("getDataset",
                                            "findOne",
                                            DATA_ACCESS_LOG_2_ENTRY("query",
                                                                    "prj",
                                                                    query.toString(),
                                                                    prj.toString()));)
        //remove the field of the document
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      query,
                                      &prj))) {
            MDBCUDA_ERR << "Error fetching dataset";
        } else if(result.isEmpty()) {
            MDBCUDA_ERR << "No element found";
        } else {
            //we have dataset so set it directly within the cdsta wrapper
            *dataset_description = new CDataWrapper(result.getObjectField(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION).objdata());
        }
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBControlUnitDataAccess::setCommandDescription(chaos::common::data::CDataWrapper& command_description) {
    
}

int MongoDBControlUnitDataAccess::setInstanceDescription(const std::string& cu_unique_id,
                                                         CDataWrapper& instance_description) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObjBuilder updated_field;
    mongo::BSONObjBuilder bson_update;
    try {
        
        if(!instance_description.hasKey(NodeDefinitionKey::NODE_PARENT)) return -1;
        
        //serach criteria
        bson_find   << NodeDefinitionKey::NODE_UNIQUE_ID << cu_unique_id
        << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_CONTROL_UNIT;
        
        //add the load_at_startup field
        updated_field << NodeDefinitionKey::NODE_PARENT << instance_description.getStringValue(NodeDefinitionKey::NODE_PARENT)
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
                if(attr_desc->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME) &&
                   attr_desc->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE)) {
                    int size;
                    CDataWrapper a;
                    a.addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, attr_desc->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME));
                    a.addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE, attr_desc->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE));
                    if(attr_desc->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) a.addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE, attr_desc->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
                    if(attr_desc->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) a.addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE, attr_desc->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE));
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
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     update))) {
            MDBCUDA_ERR << "Error updating unit server";
        }
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBControlUnitDataAccess::searchInstanceForUnitServer(std::vector<boost::shared_ptr<CDataWrapper> >& result_page,
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
    try{
        //filter on sequence, type and unit server
        bson_find_and << BSON(NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_CONTROL_UNIT);
        bson_find_and << BSON(boost::str(boost::format("instance_description.%1%") % NodeDefinitionKey::NODE_PARENT) << unit_server_uid);
        
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
        // mongo::BSONObj p =  BSON(NodeDefinitionKey::NODE_UNIQUE_ID<<1);
        DEBUG_CODE(MDBCUDA_DBG<<log_message("getInstanceDescription",
                                            "findOne",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.toString()));)
        //perform the search for the query page
        if((err = performPagedQuery(paged_result,
                                    MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
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
                    result_intance->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, it->getStringField(NodeDefinitionKey::NODE_UNIQUE_ID));
                    if(it->hasField(NodeDefinitionKey::NODE_RPC_ADDR))result_intance->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, it->getStringField(NodeDefinitionKey::NODE_RPC_ADDR));
                    if(it->hasField(NodeDefinitionKey::NODE_RPC_DOMAIN))result_intance->addStringValue(NodeDefinitionKey::NODE_RPC_DOMAIN, it->getStringField(NodeDefinitionKey::NODE_RPC_DOMAIN));
                    
                    mongo::BSONObj instance_description = it->getObjectField("instance_description");
                    result_intance->addStringValue("control_unit_implementation", instance_description.getStringField("control_unit_implementation"));
                    
                    result_page.push_back(result_intance);
                }
            }
        }
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBControlUnitDataAccess::getInstanceDescription(const std::string& control_unit_uid,
                                                         CDataWrapper **result) {
    return getInstanceDescription("", control_unit_uid, result);
}

int MongoDBControlUnitDataAccess::getInstanceDescription(const std::string& unit_server_uid,
                                                         const std::string& control_unit_uid,
                                                         CDataWrapper **result) {
    int err = 0;
    mongo::BSONObj          q_result;
    mongo::BSONObjBuilder   bson_find;
    SearchResult            paged_result;
    try{
        bson_find << NodeDefinitionKey::NODE_UNIQUE_ID << control_unit_uid;
        if(unit_server_uid.size()>0) {
            //ad also unit server for search the instance description
            bson_find << boost::str(boost::format("instance_description.%1%") % NodeDefinitionKey::NODE_PARENT) << unit_server_uid;
        }
        mongo::BSONObj q = bson_find.obj();
        
        DEBUG_CODE(MDBCUDA_DBG<<log_message("getInstanceDescription",
                                            "findOne",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    q.toString()));)
        
        if((err = connection->findOne(q_result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      q))){
            MDBCUDA_ERR << "Error calling performPagedQuery with error" << err;
        } else if(q_result.isEmpty()){
            MDBCUDA_DBG << "No instance description found for control unit:" <<control_unit_uid << " with parent:" << unit_server_uid;
        } else {
            mongo::BSONObj instance_description = q_result.getObjectField("instance_description");
            *result = new CDataWrapper();
            (*result)->addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, q_result.getStringField(NodeDefinitionKey::NODE_UNIQUE_ID));
            
            
            (*result)->addStringValue(NodeDefinitionKey::NODE_PARENT, instance_description.getStringField(NodeDefinitionKey::NODE_PARENT));
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
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBControlUnitDataAccess::deleteInstanceDescription(const std::string& unit_server_uid,
                                                            const std::string& control_unit_uid) {
    int err = 0;
    mongo::BSONObj          q_result;
    mongo::BSONObjBuilder   bson_find;
    try{
        bson_find << NodeDefinitionKey::NODE_UNIQUE_ID << control_unit_uid;
        bson_find << boost::str(boost::format("instance_description.%1%") % NodeDefinitionKey::NODE_PARENT) << unit_server_uid;
        mongo::BSONObj q = bson_find.obj();
        mongo::BSONObj u = BSON("$unset" << BSON("instance_description" << ""));
        
        DEBUG_CODE(MDBCUDA_DBG<<log_message("deleteInstanceDescription",
                                            "update",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    q.toString(),
                                                                    u.jsonString()));)
        //remove the field of the document
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     q,
                                     u))) {
            MDBCUDA_ERR << "Error removing control unit instance from node";
        }
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBControlUnitDataAccess::getInstanceDatasetAttributeDescription(const std::string& control_unit_uid,
                                                                          const std::string& attribute_name,
                                                                          boost::shared_ptr<chaos::common::data::CDataWrapper>& result) {
    int err = 0;
    mongo::BSONObj  result_bson;
    const std::string dotted_dataset_path =  boost::str(boost::format("%1%.%1%")%ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
    const std::string dotted_dataset_path_proj =  boost::str(boost::format("%1%.%1%.$")%ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
    try{
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << control_unit_uid <<
                                    NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_CONTROL_UNIT <<
                                    dotted_dataset_path << BSON("$elemMatch"<<BSON(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME << attribute_name)));
        mongo::BSONObj prj = BSON(dotted_dataset_path_proj << 1);
        
        DEBUG_CODE(MDBCUDA_DBG<<log_message("getInstanceDatasetAttributeConfiguration",
                                            "findOne",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Projection",
                                                                    query.toString(),
                                                                    prj.jsonString()));)
        //remove the field of the document
        if((err  = connection->findOne(result_bson,
                                       MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                       query,
                                       &prj))){
            
        }else if(result_bson.isEmpty()){
            MDBCUDA_ERR << "No attribute has bee foundt";
        } else {
            std::vector<mongo::BSONElement> result_array = result_bson.getFieldDotted(dotted_dataset_path).Array();
            if(result_array.size()!=0) {
                mongo::BSONObj attribute_config = result_array[0].Obj();
                //we have found description and need to return all field
                result.reset(new CDataWrapper(attribute_config.objdata()));
            }
        }
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;

}

int MongoDBControlUnitDataAccess::getInstanceDatasetAttributeConfiguration(const std::string& control_unit_uid,
                                                                           const std::string& attribute_name,
                                                                           boost::shared_ptr<chaos::common::data::CDataWrapper>& result) {
    int err = 0;
    mongo::BSONObj  result_bson;
    try{
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << control_unit_uid <<
                                    NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_CONTROL_UNIT <<
                                    "instance_description.attribute_value_descriptions" << BSON("$elemMatch"<<BSON(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME << attribute_name)));
        mongo::BSONObj prj = BSON("instance_description.attribute_value_descriptions.$" << 1);
        
        DEBUG_CODE(MDBCUDA_DBG<<log_message("getInstanceDatasetAttributeConfiguration",
                                            "findOne",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Projection",
                                                                    query.toString(),
                                                                    prj.jsonString()));)
        //remove the field of the document
        if((err  = connection->findOne(result_bson,
                                       MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                       query,
                                       &prj))){
            
        }else if(result_bson.isEmpty()){
            MDBCUDA_ERR << "No attribute has bee foundt";
        } else {
            std::vector<mongo::BSONElement> result_array = result_bson.getFieldDotted("instance_description.attribute_value_descriptions").Array();
            if(result_array.size()!=0) {
                mongo::BSONObj attribute_config = result_array[0].Obj();
                //we have found description
                result.reset(new CDataWrapper());
                if(attribute_config.hasField(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME)) {
                    result->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME,
                                           attribute_config.getStringField(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME));
                }
                if(attribute_config.hasField(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE)) {
                    result->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE,
                                           attribute_config.getStringField(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE));
                }
                if(attribute_config.hasField(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) {
                    result->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE,
                                           attribute_config.getStringField(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE));
                }
                if(attribute_config.hasField(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)) {
                    result->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE,
                                           attribute_config.getStringField(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE));
                }
            }
        }
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

//! return the data service associater to control unit
int MongoDBControlUnitDataAccess::getDataServiceAssociated(const std::string& cu_uid,
                                                           std::vector<std::string>& associated_ds){
    CHAOS_ASSERT(node_data_access)
    int err = 0;
    std::vector<mongo::BSONObj> result;
    try{
        mongo::BSONObj q = BSON(NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_DATA_SERVICE
                                << "cu_association" << BSON("$in" << BSON_ARRAY(cu_uid)));
        mongo::BSONObj p = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << 1);
        DEBUG_CODE(MDBCUDA_DBG<<log_message("getDataServiceAssociated",
                                            "find",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "projection",
                                                                    q.toString(),
                                                                    p.jsonString()));)
        //remove the field of the document
        connection->findN(result,
                          MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                          q,
                          10,
                          0,
                          &p);
        
        BOOST_FOREACH(mongo::BSONObj element, result){
            if(!element.hasField(NodeDefinitionKey::NODE_UNIQUE_ID)) continue;
            const std::string ds_unique_id = element.getStringField(NodeDefinitionKey::NODE_UNIQUE_ID);
            CDataWrapper *node_description;
            if((err = node_data_access->getNodeDescription(ds_unique_id,
                                                           &node_description))){
                MDBCUDA_ERR << "Error fetching node description for data service:" << ds_unique_id;
            } else if(node_description!=NULL){
                //we have object description
                std::auto_ptr<CDataWrapper> node_ptr(node_description);
                associated_ds.push_back(node_ptr->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID));
            } else {
                MDBCUDA_ERR << "no node description found for data service:" << ds_unique_id;
            }
        }
    } catch (const mongo::DBException &e) {
        MDBCUDA_ERR << e.what();
        err = -1;
    } catch (const CException &e) {
        MDBCUDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}