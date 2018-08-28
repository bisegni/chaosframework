/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include "MongoDBDataServiceDataAccess.h"
#include "mongo_db_constants.h"

#include <chaos/common/utility/TimingUtil.h>

#include <mongo/client/dbclient.h>

#define MDBDSDA_INFO INFO_LOG(MongoDBDataServiceDataAccess)
#define MDBDSDA_DBG  DBG_LOG(MongoDBDataServiceDataAccess)
#define MDBDSDA_ERR  ERR_LOG(MongoDBDataServiceDataAccess)

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::persistence::mongodb;

MongoDBDataServiceDataAccess::MongoDBDataServiceDataAccess(const ChaosSharedPtr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
node_data_access(NULL){}

MongoDBDataServiceDataAccess::~MongoDBDataServiceDataAccess() {}

//inherited method
int MongoDBDataServiceDataAccess::checkPresence(const std::string& ds_unique_id,
                                                bool& presence) {
    CHAOS_ASSERT(node_data_access)
    return node_data_access->checkNodePresence(presence,
                                               ds_unique_id,
                                               chaos::NodeType::NODE_TYPE_DATA_SERVICE);
    
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
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                      query,
                                      &prj))){
            MDBDSDA_ERR << "Error fecthing specific data service attribute for" << ds_unique_id << " with error:" << err;
        } else if(result.isEmpty()){
            err = -3;
            MDBDSDA_ERR << "no specific data service attribute found for " << ds_unique_id << " with error:" << err;
        } else {
            (*node_description)->addInt32Value(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT,
                                                result.getField(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT).numberInt());
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

int MongoDBDataServiceDataAccess::registerNode(const std::string& ds_zone,
                                               const std::string& ds_unique_id,
                                               const std::string& ds_direct_io_addr,
                                               uint32_t endpoint) {
    int err = 0;
    CHAOS_ASSERT(node_data_access)

    try {
        //now update proprietary fields
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << ds_unique_id
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_DATA_SERVICE);
        
        mongo::BSONObj update = BSON("$set" << BSON(NodeDefinitionKey::NODE_DIRECT_IO_ADDR << ds_direct_io_addr <<
                                                    DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT << endpoint <<
                                                    DataServiceNodeDefinitionKey::DS_HA_ZONE << ds_zone));
        
        DEBUG_CODE(MDBDSDA_DBG<<log_message("registerNode",
                                            "update",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    query.toString(),
                                                                    update.jsonString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     update,
                                     true,
                                     false,
                                     &mongo::WriteConcern::journaled))){
            MDBDSDA_ERR << "Error updating proprietary field for data service:" << ds_unique_id << " with error:" << err;
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

int MongoDBDataServiceDataAccess::updateNodeStatistic(const std::string& ds_unique_id,
                                                      const std::string& ds_direct_io_addr,
                                                      const uint32_t endpoint,
                                                      const ProcStat& process_resuorce_usage,
                                                      const std::string& ds_zone) {
    CHAOS_ASSERT(node_data_access)
    int err = 0;
    try {
        //now update proprietary fields
        mongo::BSONObj query = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << ds_unique_id
                                    << NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_DATA_SERVICE);
        
        mongo::BSONObj update = BSON("$set" << BSON(
                                         NodeDefinitionKey::NODE_DIRECT_IO_ADDR << ds_direct_io_addr <<
                                         DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT << endpoint <<
                                         DataServiceNodeDefinitionKey::DS_HA_ZONE << ds_zone<<
                                                    NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME <<(long long )process_resuorce_usage.uptime <<
													NodeHealtDefinitionKey::NODE_HEALT_USER_TIME << process_resuorce_usage.usr_time <<
													NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME << process_resuorce_usage.sys_time <<
													NodeHealtDefinitionKey::NODE_HEALT_PROCESS_SWAP << process_resuorce_usage.swap_rsrc <<
													NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP << mongo::Date_t(TimingUtil::getTimeStamp())));
        
        DEBUG_CODE(MDBDSDA_DBG<<log_message("updateExisting",
                                            "update",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Update",
                                                                    query.toString(),
                                                                    update.jsonString()));)
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                                     query,
                                     update,
                                     true,
                                     false))){
            MDBDSDA_ERR << "Error updating proprietary field for data service:" << ds_unique_id << " with error:" << err;
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
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
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
        
        if((err = connection->update(MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
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
                                                          std::vector<ChaosSharedPtr<CDataWrapper> >& node_associated) {
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
                                      MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
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
                        node_associated.push_back(ChaosSharedPtr<CDataWrapper>(node_element_description));
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

int MongoDBDataServiceDataAccess::searchAllDataAccess(std::vector<ChaosSharedPtr<common::data::CDataWrapper> >&  node_associated,
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
                                MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
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
                node_associated.push_back(ChaosSharedPtr<common::data::CDataWrapper>(new CDataWrapper(it->objdata())));
            }
        }
    }
    return err;
}

int MongoDBDataServiceDataAccess::getBestNDataService(const std::string& ds_zone,
                                                      std::vector<ChaosSharedPtr<common::data::CDataWrapper> >&  best_available_data_service,
                                                      unsigned int number_of_result) {
    int err = 0;
    SearchResult paged_result;
    
    //almost we need toreturn one data service
    if(number_of_result == 0) return 0;
    try{
        mongo::Query query = BSON(DataServiceNodeDefinitionKey::DS_HA_ZONE << ds_zone <<
                                  NodeDefinitionKey::NODE_TYPE << NodeType::NODE_TYPE_DATA_SERVICE <<
                                    NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP << BSON("$gte" << mongo::Date_t(TimingUtil::getTimestampWithDelay(5000, false))));
        //filter on sequence
        mongo::BSONObj projection = BSON(NodeDefinitionKey::NODE_UNIQUE_ID << 1 <<
                                         NodeDefinitionKey::NODE_RPC_ADDR << 1 <<
                                         NodeDefinitionKey::NODE_RPC_DOMAIN << 1 <<
                                         NodeDefinitionKey::NODE_DIRECT_IO_ADDR << 1 <<
                                         DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT << 1);
        
        
        DEBUG_CODE(MDBDSDA_DBG<<log_message("getBestNDataService",
                                            "findOne",
                                            DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                    "Projection",
                                                                    query.toString(),
                                                                    projection.toString()));)
        
        //perform the search for the query page
        connection->findN(paged_result,
                          MONGO_DB_COLLECTION_NAME(MONGODB_COLLECTION_NODES),
                          query.sort(BSON(NodeHealtDefinitionKey::NODE_HEALT_USER_TIME << 1 <<
                                          NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME << 1)),
                          number_of_result);
        //fill reuslt
        if(paged_result.size()>0) {
            //has been found
            for(SearchResultIterator it = paged_result.begin(),
                end = paged_result.end();
                it != end;
                it++) {
                //add element to result
                best_available_data_service.push_back(ChaosSharedPtr<common::data::CDataWrapper>(new CDataWrapper(it->objdata())));
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

int MongoDBDataServiceDataAccess::getBestNDataService(const std::string& ds_zone,
                                                      std::vector<std::string >&  best_available_data_service,
                                                      unsigned int number_of_result) {
    int err = 0;
    std::vector<ChaosSharedPtr<common::data::CDataWrapper> > best_available_server;
    
    if((err = getBestNDataService(ds_zone,
                                  best_available_server,
                                  number_of_result))) {
        return err;
    }
    
    //we can fill the vector with the endpoint
    for(std::vector<ChaosSharedPtr<common::data::CDataWrapper> >::iterator it = best_available_server.begin();
        it != best_available_server.end();
        it++) {
        //add address
        if((*it)->hasKey(NodeDefinitionKey::NODE_DIRECT_IO_ADDR) &&
           (*it)->hasKey(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT)){
            //get info
            if((*it)->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) {
                best_available_data_service.push_back((*it)->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID));
            }
        }
    }
    return err;
}

int MongoDBDataServiceDataAccess::getBestNDataServiceEndpoint(const std::string& ds_zone,
                                                              std::vector<std::string>&  best_available_data_service_endpoint,
                                                              unsigned int number_of_result) {
    int err = 0;
    std::vector<ChaosSharedPtr<common::data::CDataWrapper> > best_available_server;
    
    if((err = getBestNDataService(ds_zone,
                                  best_available_server,
                                  number_of_result))) {
        return err;
    }
    
    //we can fill the vector with the endpoint
    for(std::vector<ChaosSharedPtr<common::data::CDataWrapper> >::iterator it = best_available_server.begin();
        it != best_available_server.end();
        it++) {
        //add address
        if((*it)->hasKey(NodeDefinitionKey::NODE_DIRECT_IO_ADDR) &&
           (*it)->hasKey(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT)){
            //get info
            std::string node_uid = ((*it)->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)?(*it)->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID):"Not found");
            std::string direct_io_addr = (*it)->getStringValue(NodeDefinitionKey::NODE_DIRECT_IO_ADDR);
            int direct_io_endpoint = (*it)->getInt32Value(DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT);
            //add complete address
            best_available_data_service_endpoint.push_back(boost::str(boost::format("%1%|%2%") % direct_io_addr % direct_io_endpoint));
        }
    }
    return err;
}
