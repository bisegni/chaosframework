/*
 *	GetBestEndpoints.cpp
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

#include "GetBestEndpoints.h"

#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::data_service;
using namespace chaos::metadata_service::persistence::data_access;

#define GBE_INFO INFO_LOG(GetBestEndpoints)
#define GBE_DBG  DBG_LOG(GetBestEndpoints)
#define GBE_ERR  ERR_LOG(GetBestEndpoints)

GetBestEndpoints::GetBestEndpoints():
AbstractApi("getBestEndpoints"){
    
}

GetBestEndpoints::~GetBestEndpoints() {
    
}

chaos::common::data::CDataWrapper *GetBestEndpoints::execute(chaos::common::data::CDataWrapper *api_data,
                                                             bool& detach_data) throw(chaos::CException) {
    
    int err = 0;
    int32_t numner_or_result = 3;
    auto_ptr<CDataWrapper> result;
    std::vector<boost::shared_ptr<CDataWrapper> > data_services;
    
    if(api_data && api_data->hasKey("count")) {
        numner_or_result = api_data->getInt32Value("count");
    }
    
    GET_DATA_ACCESS(DataServiceDataAccess, ds_da, -1)
    
    if((err = ds_da->getBestNDataService(data_services,
                                         numner_or_result))) {
        LOG_AND_TROW(GBE_ERR, err, "Error fetching best available data service")
    }
    
    //constructs the result
    result.reset(new CDataWrapper());
    if(data_services.size()>0) {
        BOOST_FOREACH(boost::shared_ptr<CDataWrapper> ds_element, data_services) {
            if(ds_element->hasKey(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR) &&
               ds_element->hasKey(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT)){
                result->appendStringToArray(boost::str(boost::format("%1%|%2%")%
                                                       ds_element->getStringValue(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR)%
                                                       ds_element->getInt32Value(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT)));
            }
            
        }
        result->finalizeArrayForKey(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST);
    }
    return result.release();
}