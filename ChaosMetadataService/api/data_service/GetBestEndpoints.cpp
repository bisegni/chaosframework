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

#include "GetBestEndpoints.h"
#include "../../ChaosMetadataService.h"

#include <chaos/common/utility/TimingUtil.h>

#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::metadata_service::api::metadata_service;
using namespace chaos::metadata_service::persistence::data_access;

#define GBE_INFO INFO_LOG(GetBestEndpoints)
#define GBE_DBG  DBG_LOG(GetBestEndpoints)
#define GBE_ERR  ERR_LOG(GetBestEndpoints)

//!next update timestamp for the cache
static int64_t nu_cache_ts = 0;
static std::vector< ChaosSharedPtr<CDataWrapper> > data_services;
static ChaosSharedMutex smutex;

CHAOS_MDS_DEFINE_API_CLASS_CD(GetBestEndpoints, "getBestEndpoints");

CDWUniquePtr GetBestEndpoints::execute(CDWUniquePtr api_data) {
    //lock for manage the cache of endpoint
    ChaosWriteLock wl(smutex);
    
    int err = 0;
    int32_t numner_or_result = 3;
    int64_t now = TimingUtil::getTimeStamp();
    
    const std::string& ha_zone_name = ChaosMetadataService::getInstance()->setting.ha_zone_name;
    CreateNewDataWrapper(result, );
    
    if(api_data && api_data->hasKey("count")) {
        numner_or_result = api_data->getInt32Value("count");
    }
    
    if(now >= nu_cache_ts ||
       data_services.size() == 0) {
        data_services.clear();
        GET_DATA_ACCESS(DataServiceDataAccess, ds_da, -1)
        
        if((err = ds_da->getBestNDataService(ha_zone_name,
                                             data_services,
                                             numner_or_result))) {
            LOG_AND_TROW(GBE_ERR, err, "Error fetching best available data service")
        }
        //update cache on first call after ten seconds
        nu_cache_ts = now + 10000;
    }
    
    //constructs the result
    result.reset(new CDataWrapper());
    if(data_services.size()>0) {
        GBE_INFO << CHAOS_FORMAT("Found %1% data services available", %data_services.size());
        BOOST_FOREACH(ChaosSharedPtr<CDataWrapper> ds_element, data_services) {
            if(ds_element->hasKey(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR) &&
               ds_element->hasKey(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT)){
                result->appendStringToArray(boost::str(boost::format("%1%|%2%")%
                                                       ds_element->getStringValue(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR)%
                                                       ds_element->getInt32Value(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT)));
            }
            
        }
        result->finalizeArrayForKey(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST);
        
        //add rpc information for found nodes
        BOOST_FOREACH(ChaosSharedPtr<CDataWrapper> ds_element, data_services) {
            if(ds_element->hasKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR)){
                result->appendStringToArray(ds_element->getStringValue(chaos::NodeDefinitionKey::NODE_RPC_ADDR));
            }
        }
        result->finalizeArrayForKey(chaos::NodeDefinitionKey::NODE_RPC_ADDR);
    } else {
        LOG_AND_TROW(GBE_ERR, -1, "No best endpoint found");
    }
    return result;
}
