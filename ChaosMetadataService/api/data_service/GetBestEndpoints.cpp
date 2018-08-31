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
#include <boost/format.hpp>
#include <boost/foreach.hpp>

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::data_service;
using namespace chaos::metadata_service::persistence::data_access;

#define GBE_INFO INFO_LOG(GetBestEndpoints)
#define GBE_DBG  DBG_LOG(GetBestEndpoints)
#define GBE_ERR  ERR_LOG(GetBestEndpoints)

CHAOS_MDS_DEFINE_API_CD(GetBestEndpoints, getBestEndpoints);

CDWUniquePtr GetBestEndpoints::execute(CDWUniquePtr api_data) {
    int err = 0;
    int32_t numner_or_result = 3;
    const std::string& ha_zone_name = ChaosMetadataService::getInstance()->setting.ha_zone_name;
    CreateNewDataWrapper(result, );
    std::vector<ChaosSharedPtr<CDataWrapper> > data_services;
    
    if(api_data && api_data->hasKey("count")) {
        numner_or_result = api_data->getInt32Value("count");
    }
    
    GET_DATA_ACCESS(DataServiceDataAccess, ds_da, -1)
    
    if((err = ds_da->getBestNDataService(ha_zone_name,
                                         data_services,
                                         numner_or_result))) {
        LOG_AND_TROW(GBE_ERR, err, "Error fetching best available data service")
    }
    
    //constructs the result
    result.reset(new CDataWrapper());
    if(data_services.size()>0) {
        BOOST_FOREACH(ChaosSharedPtr<CDataWrapper> ds_element, data_services) {
            if(ds_element->hasKey(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR) &&
               ds_element->hasKey(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT)){
                result->appendStringToArray(boost::str(boost::format("%1%|%2%")%
                                                       ds_element->getStringValue(chaos::NodeDefinitionKey::NODE_DIRECT_IO_ADDR)%
                                                       ds_element->getInt32Value(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_ENDPOINT)));
            }
            
        }
        result->finalizeArrayForKey(chaos::DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST);
    }
    return result;
}
