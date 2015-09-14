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


#include "NodeSearch.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

using namespace chaos::common::data;

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

NodeSearch::NodeSearch(const std::string &node_uid,
					   uint32_t page_length):
	node_uid(node_uid),
	page_length(page_length){}

void NodeSearch::testSearch(){
  bool end = false;
  //the result of api
  ApiProxyResult search_result;

  //the last sequecne id
  uint32_t last_sequence_id = 0;

  //get the api proxy
  NodeSearch *node_search_api_proxy = ChaosMetadataServiceClient::getInstance()->getApiProxy<NodeSearch>(2000);

  while (!end){
	search_result = node_search_api_proxy->execute(node_uid,
												   0,
												   last_sequence_id,
												   page_length);
	if(search_result->wait()) {
		//something as been received
		if(search_result->getError()) {
		  end = true;
		  std::cerr << "Fetch result error:" << search_result->getError() << "[" << search_result->getErrorMessage() << ", " << search_result->getErrorDomain() << "]" << std::endl;
		} else {
		  //result found
		  if(search_result->getResult()->hasKey("node_search_result_page")&&
			  search_result->getResult()->isVectorValue("node_search_result_page")){
			//we have result
			std::auto_ptr<CMultiTypeDataArrayWrapper> node_found_vec(search_result->getResult()->getVectorValue("node_search_result_page"));
			for(int idx = 0;
				idx < node_found_vec->size();
				idx++) {
			  std::auto_ptr<CDataWrapper> node_found(node_found_vec->getCDataWrapperElementAtIndex(idx));

			  //print the node uid
			  std::cout << node_found->getStringValue(chaos::NodeDefinitionKey::NODE_UNIQUE_ID);

			  //get the node sequence
			  last_sequence_id  = node_found->getUInt64Value("seq");
			}
		  }
		}
	} else {
	  //print the error
	  std::cerr << "No data has been received during search paging" << std::endl;;
	  end = true;
	}
  }

}
