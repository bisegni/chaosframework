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

#include <chaos_metadata_service_client/api_proxy/logging/SearchLogEntry.h>
#include <chaos/common/message/MultiAddressMessageRequestFuture.h>
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::logging;

API_PROXY_CD_DEFINITION(SearchLogEntry,
                        MetadataServerLoggingDefinitionKeyRPC::ACTION_NODE_LOGGING_RPC_DOMAIN,
                        "searchLogEntry")

ApiProxyResult SearchLogEntry::execute(const std::string& search_string,
                                       const LogDomainList& domain_list,
                                       const uint64_t start_ts,
                                       const uint64_t end_ts,
                                       const uint64_t last_sequence_id,
                                       const uint32_t page_length) {
    CDWUniquePtr pack(new CDataWrapper());
    pack->addStringValue("search_string", search_string);
    if(last_sequence_id ) {pack->addInt64Value("seq", last_sequence_id);}
    if(start_ts){pack->addInt64Value("start_ts", start_ts);}
    if(end_ts){pack->addInt64Value("end_ts", end_ts);}
    if(domain_list.size()) {
        for(LogDomainListConstIterator it= domain_list.begin();
            it!= domain_list.end();
            it++) {
            pack->appendStringToArray(*it);
        }
        pack->finalizeArrayForKey(MetadataServerLoggingDefinitionKeyRPC::PARAM_NODE_LOGGING_LOG_DOMAIN);
    }
    pack->addInt32Value("page_length", page_length);
    return callApi(pack);
}

ChaosUniquePtr<SearchLogEntryHelper> SearchLogEntry::getHelper(CDataWrapper *api_result) {
    return ChaosUniquePtr<SearchLogEntryHelper>(new SearchLogEntryHelper(api_result));
}

SearchLogEntryHelper::SearchLogEntryHelper(CDataWrapper *api_result):
GetLogForSourceUIDHelper(api_result){}

SearchLogEntryHelper::~SearchLogEntryHelper(){}
