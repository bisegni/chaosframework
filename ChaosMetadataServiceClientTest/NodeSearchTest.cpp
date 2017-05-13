/*
 *	NodeSearch
 *	!CHAOS
 *	Created by Claudio Bisegni on 14/09/15.
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


#include "NodeSearchTest.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

using namespace chaos::common::data;

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

NodeSearchTest::NodeSearchTest(uint32_t page_length):
page_length(page_length){}

void NodeSearchTest::testSearch(const std::string& search_string){
    bool end = false;
    //the result of api
    ApiProxyResult search_result;
    
    //the last sequecne id
    uint64_t last_sequence_id = 0;
    
    //get the api proxy
    NodeSearch *node_search_api_proxy = ChaosMetadataServiceClient::getInstance()->getApiProxy<NodeSearch>(500);
    
    while (!end){
        
        search_result = node_search_api_proxy->execute(search_string,
                                                       0,
                                                       (uint32_t)last_sequence_id,
                                                       page_length);
        if(search_result->wait()) {
            usleep(500000);
            //something as been received
            if(search_result->getError()) {
                end = true;
                std::cerr << "Fetch result error:" << search_result->getError() << "[" << search_result->getErrorMessage() << ", " << search_result->getErrorDomain() << "]" << std::endl;
            } else {
                if(search_result->getResult()) {
                    //result found
                    if(search_result->getResult()->hasKey("node_search_result_page")&&
                       search_result->getResult()->isVectorValue("node_search_result_page")){
                        //we have result
                        std::unique_ptr<CMultiTypeDataArrayWrapper> node_found_vec(search_result->getResult()->getVectorValue("node_search_result_page"));
                        if(node_found_vec->size() == 0) {
                            end=true;
                        } else{
                            for (int idx = 0;
                                 idx < node_found_vec->size();
                                 idx++){
                                std::unique_ptr<CDataWrapper> node_found(node_found_vec->getCDataWrapperElementAtIndex(idx));
                                
                                //print the node uid
                                std::cout << node_found->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID) << std::endl;
                                
                                //get the node sequence
                                last_sequence_id = node_found->getUInt64Value("seq");
                            }
                        }
                    }
                } else {
                    end = true;
                }
            }
        } else {
            //print the error
            std::cerr << "No data has been received during search paging" << std::endl;
            end = true;
        }
    }
    
}
