/*
 *	HealtNodeWelcome.cpp
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

#include "ProcessHello.h"

#define H_NW_INFO INFO_LOG(ProcessHello)
#define H_NW_DBG  DBG_LOG(ProcessHello)
#define H_NW_ERR  ERR_LOG(ProcessHello)

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::healt;
using namespace chaos::metadata_service::persistence::data_access;

typedef std::vector< boost::shared_ptr<CDataWrapper> > ResultVector;

ProcessHello::ProcessHello():
AbstractApi("processHello"){

}

ProcessHello::~ProcessHello() {

}

chaos::common::data::CDataWrapper *ProcessHello::execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    bool presence = false;
    std::unique_ptr<chaos::common::data::CDataWrapper> result;
    CHECK_CDW_THROW_AND_LOG(api_data, H_NW_ERR, -1, "No parameter found")
    CHECK_KEY_THROW_AND_LOG(api_data, NodeDefinitionKey::NODE_UNIQUE_ID, H_NW_ERR, -2, "The ndk_unique_id key is mandatory")

    GET_DATA_ACCESS(NodeDataAccess, n_da, -3);
    GET_DATA_ACCESS(DataServiceDataAccess, ds_da, -4);

    const std::string h_uid = api_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
//    if((err = n_da->checkNodePresence(presence, h_uid))) {
//        LOG_AND_TROW(H_NW_ERR, err, boost::str(boost::format("Error fetching the presence of the healt process:%1%") % h_uid));
//    }else if(presence) {
//            //update found node
//        if((err = n_da->updateNode(*api_data))) {
//            LOG_AND_TROW(H_NW_ERR, err, boost::str(boost::format("Error creating the node for the healt process:%1%") % h_uid));
//        }
//    }else {            //create new node
//        if((err = n_da->insertNewNode(*api_data))) {
//            LOG_AND_TROW(H_NW_ERR, err, boost::str(boost::format("Error creating the node for the healt process:%1%") % h_uid));
//        }
//    }

    ResultVector best_available_server;
        //no we need to get tbest tree available cds to retun publishable address
    if((err = ds_da->getBestNDataService(best_available_server))) {
        LOG_AND_TROW(H_NW_ERR, err, boost::str(boost::format("Error fetching %2% best available data service to return to the healt process:%1%") % h_uid % 3));
    }

        //we can return result;
    if(best_available_server.size() == 0) {
        LOG_AND_TROW(H_NW_ERR, -1, "No dataservice has been found");
    }

    result.reset(new CDataWrapper());
    for(ResultVector::iterator it = best_available_server.begin();
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
            result->appendStringToArray(boost::str(boost::format("%1%|%2%") % direct_io_addr % direct_io_endpoint));
            H_NW_DBG<< boost::str(boost::format("Added the direct io address %1%|%2% belowing to the data service '%3%'") % direct_io_addr % direct_io_endpoint % node_uid);
        }
    }
    result->finalizeArrayForKey(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST);
    return result.release();
}
