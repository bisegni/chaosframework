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

#include "SearchLogEntry.h"
#include "../../common/LogUtility.h"

#define L_SLE_INFO INFO_LOG(SearchLogEntry)
#define L_SLE_DBG  DBG_LOG(SearchLogEntry)
#define L_SLE_ERR  ERR_LOG(SearchLogEntry)

using namespace chaos::common::data;
using namespace chaos::common::network;
using namespace chaos::common::event::channel;
using namespace chaos::metadata_service::common;
using namespace chaos::metadata_service::api::logging;
using namespace chaos::metadata_service::persistence::data_access;

SearchLogEntry::SearchLogEntry():
AbstractApi("searchLogEntry"){}

SearchLogEntry::~SearchLogEntry() {}

chaos::common::data::CDataWrapper *SearchLogEntry::execute(CDataWrapper *api_data, bool& detach_data) {
    int err = 0;
    
    CDataWrapper *result = NULL;
    
    GET_DATA_ACCESS(LoggingDataAccess, l_da, -4);
    
    //entry list
    LogEntryList entry_list;
    std::vector<std::string> domain_to_include;
    const std::string search_string = (api_data?api_data->getValueWithDefault<std::string>("search_string", ""):"");
    uint64_t start_ts =  (api_data?((uint64_t)CDW_GET_INT64_WITH_DEFAULT(api_data, "start_ts", 0)):0);
    uint64_t end_ts = (api_data?((uint64_t)CDW_GET_INT64_WITH_DEFAULT(api_data, "end_ts", 0)):0);
    
    if(api_data&&
       api_data->hasKey(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN)) {
        //we have domain
        if(api_data->isStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN)) {
            domain_to_include.push_back(api_data->getStringValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN));
        } else if(api_data->isVectorValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN)) {
            ChaosUniquePtr<CMultiTypeDataArrayWrapper> domain_vec(api_data->getVectorValue(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN));
            for(int idx = 0;
                idx < domain_vec->size();
                idx++){
                domain_to_include.push_back(domain_vec->getStringElementAtIndex(idx));
            }
        } else {
            LOG_AND_TROW_FORMATTED(L_SLE_ERR, -5, "Domain key '%1% 'need to be string or array of string",%MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN);
        }
    }
    
    uint32_t page_length =  (uint32_t)CDW_GET_INT32_WITH_DEFAULT(api_data, "page_length", 100);
    uint64_t start_sequence = (uint64_t)CDW_GET_INT64_WITH_DEFAULT(api_data, "seq", 0);
    
    if((err = l_da->searchEntryAdvanced(entry_list,
                                        search_string,
                                        domain_to_include,
                                        start_ts,
                                        end_ts,
                                        start_sequence,
                                        page_length))) {
        LOG_AND_TROW(L_SLE_ERR, err, "Error searching for source");
    }
    if(entry_list.size()) {
        ChaosUniquePtr<chaos::common::data::CDataWrapper> tmp_result(new CDataWrapper());
        for(LogEntryListIterator it = entry_list.begin();
            it != entry_list.end();
            it++){
            tmp_result->appendCDataWrapperToArray(*LogUtility::convertEntry(*(*it).get()));
        }
        tmp_result->finalizeArrayForKey("result_list");
        result = tmp_result.release();
    }
    return result;
}
