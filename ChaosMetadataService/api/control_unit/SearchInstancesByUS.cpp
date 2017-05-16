/*
 *	SearchInstancesByUS.cpp
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

#include "SearchInstancesByUS.h"

using namespace chaos::common::data;
using namespace chaos::metadata_service::api::control_unit;
using namespace chaos::metadata_service::persistence::data_access;

#define CU_SI_INFO INFO_LOG(SearchInstancesByUS)
#define CU_SI_DBG  DBG_LOG(SearchInstancesByUS)
#define CU_SI_ERR  ERR_LOG(SearchInstancesByUS)

SearchInstancesByUS::SearchInstancesByUS():
AbstractApi("searchInstancesByUS"){

}

SearchInstancesByUS::~SearchInstancesByUS() {

}

CDataWrapper *SearchInstancesByUS::execute(CDataWrapper *api_data,
                                       bool& detach_data) throw(chaos::CException) {
    int err = 0;
    uint32_t last_sequence_id = 0;
    uint32_t page_length = 30;
    std::vector<boost::shared_ptr<CDataWrapper> > page_result;
    std::vector<std::string> cu_type_filter;

    chaos::common::data::CDataWrapper *result = NULL;
    if(!api_data) {LOG_AND_TROW(CU_SI_ERR, -1, "Search parameter are needed");}
    if(!api_data->hasKey(chaos::NodeDefinitionKey::NODE_PARENT)) {LOG_AND_TROW(CU_SI_ERR, -2, "The ndk_parent key (representing the unit server uid) is mandatory");}

    if(api_data->hasKey("last_node_sequence_id")) {
        last_sequence_id = (uint32_t)api_data->getInt32Value("last_node_sequence_id");
        CU_SI_DBG << "Need to load a new page starting from id:" << last_sequence_id;
    }
    if(api_data->hasKey("result_page_length")) {
        page_length = (uint32_t)api_data->getInt32Value("result_page_length");
        CU_SI_DBG << "The length of the page has been set to:" << page_length;
    }

    if(api_data->hasKey("control_unit_implementation")) {
        std::auto_ptr<CMultiTypeDataArrayWrapper> cu_t_arr(api_data->getVectorValue("control_unit_implementation"));
        for(int idx = 0;
            idx < cu_t_arr->size();
            idx++) {
            cu_type_filter.push_back(cu_t_arr->getStringElementAtIndex(idx));
        }
    }

        //get node data access
    GET_DATA_ACCESS(ControlUnitDataAccess, cu_da, -2)
    if((err = cu_da->searchInstanceForUnitServer(page_result,
                                                 api_data->getStringValue(chaos::NodeDefinitionKey::NODE_PARENT),
                                                 cu_type_filter,
                                                 last_sequence_id,
                                                 page_length))) {
        LOG_AND_TROW(CU_SI_ERR, err, "Error performing search page")
    } else {
        if(page_result.size() > 0) {
            //add found element to result
            result = new CDataWrapper();
            for (std::vector<boost::shared_ptr<CDataWrapper> >::iterator it = page_result.begin();
                 it != page_result.end();
                 it++) {
                result->appendCDataWrapperToArray(*it->get());
            }
            result->finalizeArrayForKey("node_search_result_page");
        }
    }
    return result;
}
