/*
 *	BaseWANInterfacelHandler.h
 *	!CHOAS
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
#ifndef CHAOSFramework_AbstractInterfacelHandler_h
#define CHAOSFramework_AbstractInterfacelHandler_h

#include <vector>
#include <string>

#include "AbstractWANInterfaceResponse.h"
#include "../utility/TypedConstrainedHashMap.h"
#include "../api/AbstractApiGroup.h"
#include "../api/PersistenceAccessor.h"

#include <json/json.h>

namespace chaos {
    namespace wan_proxy {
        namespace wan_interface {
			
			typedef chaos::wan_proxy::utility::TypedConstrainedHashMap<api::AbstractApiGroup> ApiGroupHashMap;
			
			//! collect all group of api that respetc a specified version
			struct ApiGroupVersionDomain:
			public ApiGroupHashMap {
				ApiGroupVersionDomain();
				~ApiGroupVersionDomain();
				
				int callGroupApi(std::vector<std::string>& api_tokens,
								 const Json::Value& input_data,
								 std::map<std::string, std::string>& output_header,
								 Json::Value& output_data);
			protected:
				
				//! inherited from AbstractApiHashMap
				void clearHashTableElement(const void *key,
										   uint32_t key_len,
										   api::AbstractApiGroup *element);
			};
			
			
			typedef std::vector<ApiGroupVersionDomain*>				GroupVersionList;
			typedef std::vector<ApiGroupVersionDomain*>::iterator	GroupVersionListIterator;
			
            //! define the rule for the interface hander
            /*!
             the wan interface request will be forwarde to 
             subclass of this abstract class. Only on handler at time can be
             installed in the wan interface.
             */
			class BaseWANInterfacelHandler:
			public api::PersistenceAccessor {
				
				//! lis of all registered domain for specified version(the index of array is the version 0==v1)
				GroupVersionList api_group_version_domain_list;

            public:
                //! default constructor
                BaseWANInterfacelHandler(persistence::AbstractPersistenceDriver *_persistence_driver);
                
                //! default destructor
                ~BaseWANInterfacelHandler();
                
                //! execute a wan api
                /*!
                 WAN api follow the rest service's rules (but can be called on various protocol not only
                 http). Each api has a sequence of tokens a json input and a response. The Implementation 
                 needs to scan the token and execute the proper api forwarding the json pack and compose
                 the response.
				 \param version indicate the version of the api to call[the value is 1-based.
				 \param api_tokens identify the list of the tocken of the received api, tipically the first
				 is the name and the next tocken rpresent the parameter.
				 \param input_data is the json object representing the input data for the api.
				 \param output_header is the output header map that help to collect the header of the api result
				 \param output_data is the json objetc taht will collect the data to forward as response to the request.
                 */
                virtual int handleCall(int version,
									   std::vector<std::string>& api_tokens,
									   const Json::Value& input_data,
									   std::map<std::string, std::string>& output_header,
									   Json::Value& output_data);
				
				
				template<typename G>
				void addGroupToVersion(int version) {
					CHAOS_ASSERT(getPersistenceDriver())
					if(api_group_version_domain_list.size() < version+1) {
						//add the new version
						api_group_version_domain_list.push_back(new ApiGroupVersionDomain());
					}
					
					//add the group to the version
					api_group_version_domain_list[version]->addTypeWithParam<G, persistence::AbstractPersistenceDriver*>(getPersistenceDriver());
				}

            };
            
        }
    }
}
#endif
