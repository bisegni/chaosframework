/*
 *	MongoDBDataServiceDataAccess.cpp
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

#include "MongoDBDataServiceDataAccess.h"
#include "mongo_db_constants.h"

#include <chaos/common/utility/TimingUtil.h>

#include <mongo/client/dbclient.h>

#define MDBDSDA_INFO INFO_LOG(MongoDBDataServiceDataAccess)
#define MDBDSDA_DBG  DBG_LOG(MongoDBDataServiceDataAccess)
#define MDBDSDA_ERR  ERR_LOG(MongoDBDataServiceDataAccess)

using namespace chaos::common::data;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBDataServiceDataAccess::MongoDBDataServiceDataAccess(const boost::shared_ptr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){

}

MongoDBDataServiceDataAccess::~MongoDBDataServiceDataAccess() {

}

    //inherited method
int MongoDBDataServiceDataAccess::checkPresence(const std::string& ds_unique_id,
                                                bool& presence) {
    CHAOS_ASSERT(node_data_access)
    return node_data_access->checkNodePresence(presence,
                                               ds_unique_id,
                                               chaos::NodeType::NODE_TYPE_DATA_SERVICE);

}

    //inherited method
int MongoDBDataServiceDataAccess::insertNew(const std::string& ds_unique_id,
                                            const std::string& ds_direct_io_addr,
                                            uint32_t endpoint) {
    CHAOS_ASSERT(node_data_access)
    int err = 0;
    try {
            //first update thenode part then the unit server
            //we can insert new node
        CDataWrapper node_description;
        node_description.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, ds_unique_id);
        node_description.addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_DATA_SERVICE);
        node_description.addStringValue(NodeDefinitionKey::NODE_DIRECT_IO_ADDR, ds_direct_io_addr);
        node_description.addInt32Value(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT, endpoint);

        if((err = node_data_access->insertNewNode(node_description))){
            MDBDSDA_ERR << "Error creating new node for" << ds_unique_id << " with error:" << err;
        }
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBDataServiceDataAccess::getDescription(const std::string& ds_unique_id,
                                                 chaos::common::data::CDataWrapper **node_description) {
    CHAOS_ASSERT(node_data_access)
    int err = 0;
    if((err = node_data_access->getNodeDescription(ds_unique_id, node_description))) return err;

    mongo::BSONObj result;
    try {
            //we can update the node node
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << ds_unique_id
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_DATA_SERVICE);
        mongo::BSONObj prj = BSON(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT << 1);

        DEBUG_CODE(MDBDSDA_DBG<<log_message("getDescription",
                                            "findOne",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "prj",
                                                                    query.toString(),
                                                                    prj.jsonString()));)
        if((err = connection->findOne(result,
                                     MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(),
                                                              MONGODB_COLLECTION_NODES),
                                     query,
                                     &prj))){
            MDBDSDA_ERR << "Error fecthing specific data service attribute for" << ds_unique_id << " with error:" << err;
        } else if(result.isEmpty()){
            err = -3;
            MDBDSDA_ERR << "no specific data service attribute found for " << ds_unique_id << " with error:" << err;
        } else {
            (*node_description)->addStringValue(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT,
                                                result.getStringField(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT));
        }
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }

    return err;
}

    //inherited method
int MongoDBDataServiceDataAccess::updateExisting(const std::string& ds_unique_id,
                                                 const std::string& ds_direct_io_addr,
                                                 uint32_t endpoint) {
    CHAOS_ASSERT(node_data_access)
    int err = 0;
    try {
            //we can update the node node
        CDataWrapper node_description;
        node_description.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, ds_unique_id);
        node_description.addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_DATA_SERVICE);
        node_description.addStringValue(NodeDefinitionKey::NODE_DIRECT_IO_ADDR, ds_direct_io_addr);

        if((err = node_data_access->updateNode(node_description))){
            MDBDSDA_ERR << "Error updating data service:" << ds_unique_id << " with error:" << err;
        } else {
                //now update proprietary fields
            mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << ds_unique_id
                                        << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_DATA_SERVICE);

            mongo::BSONObj update = BSON("$set" << BSON(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT << endpoint));

            DEBUG_CODE(MDBDSDA_DBG<<log_message("updateExisting",
                                                "update",
                                                DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                        "Update",
                                                                        query.toString(),
                                                                        update.jsonString()));)

            if((err = connection->update(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(),
                                                                  MONGODB_COLLECTION_NODES),
                                         query,
                                         update))){
                MDBDSDA_ERR << "Error updating proprietary field for data service:" << ds_unique_id << " with error:" << err;
            }
        }
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

    //inherited method
int MongoDBDataServiceDataAccess::deleteDataService(const std::string& ds_unique_id) {
    CHAOS_ASSERT(node_data_access)
    int err = 0;
    try {

        if((err = node_data_access->deleteNode(ds_unique_id))){
            MDBDSDA_ERR << "Error updating data service:" << ds_unique_id << " with error:" << err;
        }
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

    //inherited method
int MongoDBDataServiceDataAccess::associateNode(const std::string& ds_unique_id,
                                                const std::string& associated_node_unique_id) {
    int err = 0;
    try {
        mongo::BSONObj q = BSON(NodeDefinitionKey::NODE_UNIQUE_ID<<ds_unique_id
                                << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_DATA_SERVICE);

        mongo::BSONObj u = BSON("$addToSet" << BSON("cu_association" << associated_node_unique_id));

        DEBUG_CODE(MDBDSDA_DBG<<log_message("associateNode",
                                            "update",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    q.toString(),
                                                                    u.jsonString()));)

        if((err = connection->update(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(),
                                                              MONGODB_COLLECTION_NODES),
                                     q,
                                     u))) {
            MDBDSDA_ERR << "Error adding to data service:" << ds_unique_id << " the control unit:" << associated_node_unique_id <<" with error:"<< err;
        }
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

    //inherited method
int MongoDBDataServiceDataAccess::removeNode(const std::string& ds_unique_id,
                                             const std::string& associated_node_unique_id) {
    int err = 0;
    try {
        mongo::BSONObj q = BSON(NodeDefinitionKey::NODE_UNIQUE_ID<<ds_unique_id
                                << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_DATA_SERVICE
                                << "cu_association" << associated_node_unique_id);

        mongo::BSONObj u = BSON("$pull" << BSON("cu_association" << associated_node_unique_id));

        DEBUG_CODE(MDBDSDA_DBG<<log_message("removeNode",
                                            "update",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    q.toString(),
                                                                    u.jsonString()));)

        if((err = connection->update(MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(),
                                                              MONGODB_COLLECTION_NODES),
                                     q,
                                     u,
                                     false,
                                     true))) {
            MDBDSDA_ERR << "Error removing from data service:" << ds_unique_id << " the control unit:" << associated_node_unique_id <<" with error:"<< err;
        }
    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

    //inherited method
int MongoDBDataServiceDataAccess::searchAssociationForUID(const std::string& ds_unique_id,
                                                          std::vector<boost::shared_ptr<CDataWrapper> >& node_associated) {
    CHAOS_ASSERT(node_data_access)
    int err = 0;
    std::vector<mongo::BSONElement> association;
    try {

        mongo::BSONObj result;

        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID<<ds_unique_id
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_DATA_SERVICE);
        mongo::BSONObj projection = BSON("cu_association" << 1);


        DEBUG_CODE(MDBDSDA_DBG<<log_message("searchAssociationForUID",
                                            "query",
                                            DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                    query.toString()));)
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(),
                                                               MONGODB_COLLECTION_NODES),
                                      query,
                                      &projection))){
            MDBDSDA_ERR << "Error finding association for data service:" << ds_unique_id << " with error:"<< err;
        }

        if(!result.isEmpty() &&
           result.hasField("cu_association")) {
                //get all array element
            association = result.getField("cu_association").Array();
                //check if not empty
            if(association.size()) {
                for(std::vector<mongo::BSONElement>::iterator it = association.begin();
                    it != association.end();
                    it++){
                    const std::string cu_associated_id = it->String();
                    CDataWrapper *node_element_description = NULL;
                    if((err = node_data_access->getNodeDescription(it->String(),
                                                                   &node_element_description))) {
                        MDBDSDA_ERR << "Error getting node description for associated cu:" << it->String() << " to the data service:" << ds_unique_id <<" with error:"<< err;
                        continue;
                    } else if(node_element_description) {
                        node_associated.push_back(boost::shared_ptr<CDataWrapper>(node_element_description));
                    }
                }
            }
        } else {
            MDBDSDA_DBG<< "No association found for data servce " << ds_unique_id;
        }

    } catch (const mongo::DBException &e) {
        MDBDSDA_ERR << e.what();
        err = -1;
    } catch (const chaos::CException &e) {
        MDBDSDA_ERR << e.what();
        err = e.errorCode;
    }
    return err;
}

int MongoDBDataServiceDataAccess::searchAllDataAccess(std::vector<boost::shared_ptr<common::data::CDataWrapper> >&  node_associated,
                                                      uint32_t last_unique_id,
                                                      uint32_t page_length) {
    int err = 0;
    mongo::BSONObjBuilder   bson_find;
    SearchResult            paged_result;

        //compose query

        //filter on sequence
    bson_find << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_DATA_SERVICE
    << "seq" << BSON("$gte"<<last_unique_id);


    mongo::BSONObj query = bson_find.obj();

    DEBUG_CODE(MDBDSDA_DBG<<log_message("searchAllDataAccess",
                                        "performPagedQuery",
                                        DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                query.jsonString()));)
    mongo::BSONObj projection = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << 1 <<
                                     NodeDefinitionKey::NODE_TYPE << 1 <<
                                     NodeDefinitionKey::NODE_RPC_ADDR << 1 <<
                                     NodeDefinitionKey::NODE_RPC_DOMAIN << 1 <<
                                     NodeDefinitionKey::NODE_DIRECT_IO_ADDR << 1 <<
                                     NodeDefinitionKey::NODE_TIMESTAMP << 1 <<
                                     DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT << 1);
        //perform the search for the query page
    if((err = performPagedQuery(paged_result,
                                MONGO_DB_COLLECTION_NAME(getDatabaseName().c_str(), MONGODB_COLLECTION_NODES),
                                query,
                                &projection,
                                NULL,
                                page_length))) {
        MDBDSDA_ERR << "Error calling performPagedQuery with error" << err;
    } else {
        DEBUG_CODE(MDBDSDA_DBG << "The query '"<< query.toString() <<"' has found " << paged_result.size() << " result";)
        if(paged_result.size()) {
            for (SearchResultIterator it = paged_result.begin();
                 it != paged_result.end();
                 it++) {
                node_associated.push_back(boost::shared_ptr<common::data::CDataWrapper>(new CDataWrapper(it->objdata())));
            }
        }
    }
    return err;
}