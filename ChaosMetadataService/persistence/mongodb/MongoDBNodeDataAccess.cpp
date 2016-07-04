/*
 *	MongoDBNodeDataAccess.cpp
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
#include <mongo/client/dbclient.h>
#include "MongoDBNodeDataAccess.h"
#include "mongo_db_constants.h"

#include <chaos/common/utility/TimingUtil.h>
#include <boost/algorithm/string.hpp>

#include <map>

#define MDBNDA_INFO INFO_LOG(MongoDBNodeDataAccess)
#define MDBNDA_DBG  DBG_LOG(MongoDBNodeDataAccess)
#define MDBNDA_ERR  ERR_LOG(MongoDBNodeDataAccess)

using namespace boost;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBNodeDataAccess::MongoDBNodeDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
utility_data_access(NULL){}

MongoDBNodeDataAccess::~MongoDBNodeDataAccess() {}

int MongoDBNodeDataAccess::getNodeDescription(const std::string& node_unique_id,
                                              chaos::common::data::CDataWrapper **node_description) {
    int err = 0;
    mongo::BSONObj result;
    mongo::BSONObjBuilder query_builder;
    try {
        if(node_unique_id.size() == 0) return -1; //invalid unique id
        query_builder << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;

        mongo::BSONObj q = query_builder.obj();
        mongo::BSONObj p = BSON(chaos::NodeDefinitionKey::NODE_UNIQUE_ID << 1 <<
                                chaos::NodeDefinitionKey::NODE_TYPE << 1 <<
                                chaos::NodeDefinitionKey::NODE_RPC_ADDR << 1 <<
                                chaos::NodeDefinitionKey::NODE_RPC_DOMAIN << 1 <<
                                chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR << 1 <<
                                chaos::NodeDefinitionKey::NODE_TIMESTAMP << 1);

        DEBUG_CODE(MDBNDA_DBG<<log_message("getNodeDescription",
                                           "findOne",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "Projection",
                                                                   q.jsonString(),
                                                                   p.jsonString()));)

        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES), q, &p))){
            MDBNDA_ERR << "Error fetching node description";
        } else if(result.isEmpty()) {
            MDBNDA_ERR << "No node description has been found";
            err = -2;
        } else {
            *node_description = new CDataWrapper(result.objdata());
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//inherited method
int MongoDBNodeDataAccess::insertNewNode(CDataWrapper& node_description) {
    int err = 0;
    uint64_t sequence_id = 0;
    try {
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) return -1;
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_TYPE)) return -2;
        if(!node_description.hasKey("seq")) {
            CHAOS_ASSERT(utility_data_access)
            if(utility_data_access->getNextSequenceValue("nodes", sequence_id)) {
                MDBNDA_ERR << "Error getting new sequence for node";
                return err;
            } else {
                node_description.addInt64Value("seq", sequence_id);
            }
        }
        //if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) return -3;
        //if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN)) return -4;
        //if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_TIMESTAMP)) return -5;

        std::auto_ptr<SerializationBuffer> ser(node_description.getBSONData());
        mongo::BSONObj obj_to_insert(ser->getBufferPtr());

        DEBUG_CODE(MDBNDA_DBG<<log_message("insertNewNode",
                                           "insert",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   obj_to_insert));)

        if((err = connection->insert(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     obj_to_insert))) {
            MDBNDA_ERR << "Error creating new node";
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//! update the node updatable feature
int MongoDBNodeDataAccess::updateNode(chaos::common::data::CDataWrapper& node_description) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObjBuilder updated_field;
    mongo::BSONObjBuilder bson_update;
    mongo::BSONArrayBuilder bson_update_array;
    try {
        if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_UNIQUE_ID)) return -1;
        //if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) return -2;
        //if(!node_description.hasKey(chaos::NodeDefinitionKey::NODE_TIMESTAMP)) return -4;

        //serach criteria
        bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_TYPE)) {
            bson_find << chaos::NodeDefinitionKey::NODE_TYPE << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_TYPE);
        }
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)) {
            updated_field << chaos::NodeDefinitionKey::NODE_RPC_ADDR << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR);
        }
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN)) {
            updated_field << chaos::NodeDefinitionKey::NODE_RPC_DOMAIN << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_RPC_DOMAIN);
        }
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR)) {
            updated_field << chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR << node_description.getStringValue(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR);
        }
        if(node_description.hasKey(chaos::NodeDefinitionKey::NODE_TIMESTAMP)) {
            updated_field << chaos::NodeDefinitionKey::NODE_TIMESTAMP << mongo::Date_t(node_description.getUInt64Value(chaos::NodeDefinitionKey::NODE_TIMESTAMP));
        }
        if(node_description.hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC)) {
            std::auto_ptr<CMultiTypeDataArrayWrapper> action_array(node_description.getVectorValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC));
            for(int idx = 0;
                idx < action_array->size();
                idx++) {
                mongo::BSONObjBuilder action_description;
                mongo::BSONArrayBuilder param_descriptions;
                std::auto_ptr<CDataWrapper> element(action_array->getCDataWrapperElementAtIndex(idx));
                if(element->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_NAME)) {
                    action_description << chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_NAME
                    << element->getStringValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_NAME);
                }
                if(element->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESCRIPTION)) {
                    action_description << chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESCRIPTION
                    << element->getStringValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESCRIPTION);
                }
                //check if the action has parameter
                if(element->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PARAM)) {
                    std::auto_ptr<CMultiTypeDataArrayWrapper> param_array(element->getVectorValue(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PARAM));
                    for(int idx = 0;
                        idx < param_array->size();
                        idx++) {
                        mongo::BSONObjBuilder single_param_desc;
                        std::auto_ptr<CDataWrapper> param(param_array->getCDataWrapperElementAtIndex(idx));

                        if(param->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_NAME)) {
                            single_param_desc << chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_NAME
                            << param->getStringValue( chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_NAME);
                        }
                        if(param->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_INFO)) {
                            single_param_desc << chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_INFO
                            << param->getStringValue( chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_INFO);
                        }
                        if(param->hasKey(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_TYPE)) {
                            single_param_desc << chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_TYPE
                            << param->getInt32Value(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PAR_TYPE);
                        }
                        //add element to array
                        param_descriptions << single_param_desc.obj();
                    }
                    //add all param description
                    action_description.appendArray(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC_PARAM, param_descriptions.arr());
                }
                //appen action description to array
                bson_update_array << action_description.obj();
            }

            //add all action to the bson update object
            updated_field.appendArray(chaos::RpcActionDefinitionKey::CS_CMDM_ACTION_DESC, bson_update_array.arr());
        }
        if(updated_field.asTempObj().isEmpty()) return 0;
        mongo::BSONObj query = bson_find.obj();
        //set the update
        bson_update << "$set" << updated_field.obj();
        mongo::BSONObj update = bson_update.obj();



        DEBUG_CODE(MDBNDA_DBG<<log_message("updateUS",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "Update",
                                                                   query.jsonString(),
                                                                   update.jsonString()));)

        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     update))) {
            MDBNDA_ERR << "Error updating unit server";
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//inherited method
int MongoDBNodeDataAccess::checkNodePresence(bool& presence,
                                             const std::string& node_unique_id,
                                             const std::string& node_unique_type) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    mongo::BSONObj result;
    try {
        bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
        if(node_unique_type.size()>0) {
            bson_find << chaos::NodeDefinitionKey::NODE_TYPE << node_unique_type;
        }
        mongo::BSONObj q = bson_find.obj();

        DEBUG_CODE(MDBNDA_DBG<<log_message("checkNodePresence",
                                           "find",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)

        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      q))){
            MDBNDA_ERR << "Error sercing unit server unit server" << node_unique_id;
        }
        presence = !result.isEmpty();
    } catch (const mongo::DBException &e) {
        presence = false;
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::deleteNode(const std::string& node_unique_id,
                                      const std::string& node_type) {
    int err = 0;
    //allocate data block on vfat
    mongo::BSONObjBuilder bson_find;
    try {
        bson_find << chaos::NodeDefinitionKey::NODE_UNIQUE_ID << node_unique_id;
        if(node_type.size()) {
            bson_find << chaos::NodeDefinitionKey::NODE_TYPE << node_type;
        }
        mongo::BSONObj q = bson_find.obj();

        DEBUG_CODE(MDBNDA_DBG<<log_message("deleteNode",
                                           "delete",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)

        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     q))){
            MDBNDA_ERR << "Error deleting unit server" << node_unique_id;
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::searchNode(chaos::common::data::CDataWrapper **result,
                                      const std::string& criteria,
                                      uint32_t search_type,
                                      uint32_t last_unique_id,
                                      uint32_t page_length) {
    int err = 0;
    //decode the type of the node
    std::string             type_of_node;
    //contain token found in criteria
    std::vector<std::string> criteria_token;
    //helper for regular expression
    std::string             token_for_mongo;
    mongo::BSONObjBuilder   bson_find;
    mongo::BSONArrayBuilder bson_find_or;
    mongo::BSONArrayBuilder bson_find_and;
    SearchResult            paged_result;

    //compose query

    //filter on sequence
    bson_find_and << BSON( "seq" << BSON("$gt"<<last_unique_id));

    //filter on type
    if(search_type>0) {
        switch(search_type) {
            case 1:
                type_of_node = chaos::NodeType::NODE_TYPE_UNIT_SERVER;
                break;
            case 2:
                type_of_node = chaos::NodeType::NODE_TYPE_CONTROL_UNIT;
                break;
            default:
                break;
        }
        bson_find_and << BSON( chaos::NodeDefinitionKey::NODE_TYPE << type_of_node);
    }

    //compose the 'or' condition for all token of unique_id filed
    bson_find_and << BSON("$or" << getSearchTokenOnFiled(criteria, chaos::NodeDefinitionKey::NODE_UNIQUE_ID));
    bson_find.appendArray("$and", bson_find_and.obj());
    mongo::BSONObj q = bson_find.obj();

    DEBUG_CODE(MDBNDA_DBG<<log_message("searchNode",
                                       "performPagedQuery",
                                       DATA_ACCESS_LOG_1_ENTRY("Query",
                                                               q.jsonString()));)

    //perform the search for the query page
    if((err = performPagedQuery(paged_result,
                                MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                q,
                                NULL,
                                NULL,
                                page_length))) {
        MDBNDA_ERR << "Error calling performPagedQuery with error" << err;
    } else {
        DEBUG_CODE(MDBNDA_DBG << "The query '"<< q.toString() <<"' has found " << paged_result.size() << " result";)
        if(paged_result.size()) {
            *result = new CDataWrapper();
            for (SearchResultIterator it = paged_result.begin();
                 it != paged_result.end();
                 it++) {
                CDataWrapper cd(it->objdata());
                (*result)->appendCDataWrapperToArray(cd);
            }
            (*result)->finalizeArrayForKey("node_search_result_page");
        }
    }
    return err;
}

int MongoDBNodeDataAccess::checkCommandPresence(const std::string& command_unique_id,
                                                bool& presence) {
    int err = 0;
    mongo::BSONObj result;
    try {
        mongo::BSONObj q = BSON(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);

        DEBUG_CODE(MDBNDA_DBG<<log_message("checkCommandPresence",
                                           "find",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)

        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND),
                                      q))){
            MDBNDA_ERR << boost::str(boost::format("Error checking presence for command uid:%1%")%command_unique_id);
        }
        presence = !result.isEmpty();
    } catch (const mongo::DBException &e) {
        presence = false;
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::setCommand(chaos::common::data::CDataWrapper& command) {
    int err = 0;
    mongo::BSONObj result;
    mongo::BSONObjBuilder update_builder;
    try {
        if(!command.hasKey(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)) {
            return -1;
            MDBNDA_ERR << "No command uid found";
        }
        if(!command.hasKey(BatchCommandAndParameterDescriptionkey::BC_ALIAS)) {
            return -2;
            MDBNDA_ERR << "No command alias found";
        }
        if(!command.hasKey(BatchCommandAndParameterDescriptionkey::BC_DESCRIPTION)) {
            return -3;
            MDBNDA_ERR << "No command description found";
        }
        std::auto_ptr<SerializationBuffer> ser(command.getBSONData());
        mongo::BSONObj i(ser->getBufferPtr());

        mongo::BSONObj query = BSON(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command.getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID));

        update_builder.append(i.getField(BatchCommandAndParameterDescriptionkey::BC_ALIAS));
        update_builder.append(i.getField(BatchCommandAndParameterDescriptionkey::BC_DESCRIPTION));
        if(i.hasElement(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS)){
            //we have parameters
            update_builder.append(i.getField(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS));
        }
        mongo::BSONObj update = BSON("$set" << update_builder.obj());

        DEBUG_CODE(MDBNDA_DBG<<log_message("updateUS",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "Update",
                                                                   query.jsonString(),
                                                                   update.jsonString()));)

        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND),
                                     query,
                                     update,
                                     true))) {
            MDBNDA_ERR << "Error updating unit server:" << err;
        }

    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::getCommand(const std::string& command_unique_id,
                                      chaos::common::data::CDataWrapper **command) {
    int err = 0;
    mongo::BSONObj result;
    try {
        //create query
        mongo::BSONObj q = BSON(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);

        DEBUG_CODE(MDBNDA_DBG<<log_message("getCommand",
                                           "findOne",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)

        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND),
                                      q))){
            MDBNDA_ERR << boost::str(boost::format("Error searching the command uid:%2%")%command_unique_id);
        } else if(result.isEmpty()) {
            MDBNDA_ERR << boost::str(boost::format("No command for uid:%2%")%command_unique_id);
        } else {
            //return a new datawrapper with the found content
            *command = new CDataWrapper(result.objdata());
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//! inherited method
int MongoDBNodeDataAccess::deleteCommand(const std::string& command_unique_id) {
    int err = 0;
    try {
        //create query
        mongo::BSONObj q = BSON(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);

        DEBUG_CODE(MDBNDA_DBG<<log_message("deleteCommandTemplate",
                                           "delete",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)

        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND),
                                     q))){
            MDBNDA_ERR << boost::str(boost::format("Error removing the command uid:%2%")%command_unique_id);
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::checkCommandTemplatePresence(const std::string& template_name,
                                                        const std::string& command_unique_id,
                                                        bool& presence) {
    int err = 0;
    mongo::BSONObj result;
    try {
        mongo::BSONObj q = BSON("template_name" << template_name <<
                                BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);

        DEBUG_CODE(MDBNDA_DBG<<log_message("checkCommandTemplatePresence",
                                           "find",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)

        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE),
                                      q))){
            MDBNDA_ERR << boost::str(boost::format("Error searching template name: %1% for command uid:%2%")%template_name%command_unique_id);
        }
        presence = !result.isEmpty();
    } catch (const mongo::DBException &e) {
        presence = false;
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::setCommandTemplate(chaos::common::data::CDataWrapper& command_template) {
    int err = 0;
    try {
        if(!command_template.hasKey("template_name") ||
           !command_template.hasKey(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)) {
            MDBNDA_ERR << boost::str(boost::format("The key 'template_name' and  '%2%' are mandatory!")%"template_name"%BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);
            return -1;
        }

        std::string template_name = command_template.getStringValue("template_name");
        std::string command_unique_id = command_template.getStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID);

        //create query
        mongo::BSONObj q = BSON("template_name" << template_name <<
                                BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);

        //create the update package (all key imnus the first two used before
        std::auto_ptr<CDataWrapper> to_update(new CDataWrapper());
        to_update->addStringValue("type", "template");
        std::vector<std::string> all_keys;
        command_template.getAllKey(all_keys);

        for(std::vector<std::string>::iterator it = all_keys.begin();
            it != all_keys.end();
            it++) {
            if((it->compare("template_name")==0) ||
               (it->compare(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID)==0)) continue;

            //copy the key if present
            command_template.copyKeyTo(*it, *to_update);
        }

        std::auto_ptr<SerializationBuffer> chaos_bson(to_update->getBSONData());
        mongo::BSONObj u = BSON("$set" << mongo::BSONObj(chaos_bson->getBufferPtr()));

        DEBUG_CODE(MDBNDA_DBG<<log_message("setCommandTemplate",
                                           "update",
                                           DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                   "update",
                                                                   q.jsonString(),
                                                                   u.jsonString()));)

        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE),
                                     q,
                                     u,
                                     true))){
            MDBNDA_ERR << boost::str(boost::format("Error setting the template name: %1% for command uid:%2%")%template_name%command_unique_id);
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::deleteCommandTemplate(const std::string& template_name,
                                                 const std::string& command_unique_id) {
    int err = 0;
    try {
        //create query
        mongo::BSONObj q = BSON("template_name" << template_name <<
                                BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);

        DEBUG_CODE(MDBNDA_DBG<<log_message("deleteCommandTemplate",
                                           "delete",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)

        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE),
                                     q))){
            MDBNDA_ERR << boost::str(boost::format("Error removing the template name: %1% for command uid:%2%")%template_name%command_unique_id);
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBNodeDataAccess::getCommandTemplate(const std::string& template_name,
                                              const std::string& command_unique_id,
                                              chaos::common::data::CDataWrapper **command_template) {
    int err = 0;
    mongo::BSONObj result;
    try {
        //create query
        mongo::BSONObj q = BSON("template_name" << template_name <<
                                BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << command_unique_id);

        DEBUG_CODE(MDBNDA_DBG<<log_message("getCommandTemplate",
                                           "findOne",
                                           DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                   q.jsonString()));)

        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE),
                                      q))){
            MDBNDA_ERR << boost::str(boost::format("Error searching the template name: %1% for command uid:%2%")%template_name%command_unique_id);
        } else if(result.isEmpty()) {
            MDBNDA_ERR << boost::str(boost::format("No template found for template name: %1% and command uid:%2%")%template_name%command_unique_id);
        } else {
            //return a new datawrapper with the found content
            *command_template = new CDataWrapper(result.objdata());
        }
    } catch (const mongo::DBException &e) {
        MDBNDA_ERR << e.what();
        err = e.getCode();
    }
    return err;
}

//! inherited method
int MongoDBNodeDataAccess::searchCommandTemplate(chaos::common::data::CDataWrapper **result,
                                                 const std::vector<std::string>& cmd_uid_to_filter,
                                                 uint32_t last_unique_id,
                                                 uint32_t page_length) {
    int err = 0;

    mongo::BSONObjBuilder   bson_find;
    mongo::BSONArrayBuilder bson_uid_array_list;
    mongo::BSONArrayBuilder bson_find_and;
    SearchResult            paged_result;

    //compose query

    //filter on sequence
    bson_find_and << BSON( "seq" << BSON("$gte"<<last_unique_id));

    //filter on type
    for(std::vector<std::string>::const_iterator it = cmd_uid_to_filter.begin();
        it != cmd_uid_to_filter.end();
        it++) {
        bson_uid_array_list << *it;
    }

    //compose the or
    bson_find_and << BSON(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << BSON("$in" << bson_uid_array_list.arr()));
    bson_find.appendArray("$and", bson_find_and.obj());
    mongo::BSONObj q = bson_find.obj();
    mongo::BSONObj p = BSON("template_name" << 1 <<
                            BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID << 1);
    DEBUG_CODE(MDBNDA_DBG<<log_message("searchCommandTemplate",
                                       "performPagedQuery",
                                       DATA_ACCESS_LOG_2_ENTRY("Query",
                                                               "Projection",
                                                               q.jsonString(),
                                                               p.jsonString()));)

    //perform the search for the query page
    if((err = performPagedQuery(paged_result,
                                MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES_COMMAND_TEMPLATE),
                                q,
                                NULL,
                                NULL,
                                page_length))) {
        MDBNDA_ERR << "Error calling performPagedQuery with error" << err;
    } else {
        DEBUG_CODE(MDBNDA_DBG << "The query '"<< q.toString() <<"' has found " << paged_result.size() << " result";)
        if(paged_result.size()) {
            *result = new CDataWrapper();
            for (SearchResultIterator it = paged_result.begin();
                 it != paged_result.end();
                 it++) {
                CDataWrapper cd(it->objdata());
                (*result)->appendCDataWrapperToArray(cd);
            }
            (*result)->finalizeArrayForKey("cmd_tmpl_search_result_page");
        }
    }
    return err;
}
