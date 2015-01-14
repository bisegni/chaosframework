/*
 *	AbstractWANInterfacelHandler.h
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

#include <json/json.h>

namespace chaos {
    namespace wan_proxy {
        namespace wan_interface {
            
            //! define the rule for the interface hander
            /*!
             the wan interface request will be forwarde to 
             subclass of this abstract class. Only on handler at time can be
             installed in the wan interface.
             */
            class AbstractWANInterfacelHandler {
            public:
                //! default constructor
                AbstractWANInterfacelHandler();
                
                //! default destructor
                ~AbstractWANInterfacelHandler();
                
                //! execute a wan api
                /*!
                 WAN api follow the rest service's rules (but can be called on various protocol not only
                 http). Each api has a sequence of tokens a json input and a response. The Implementation 
                 needs to scan the token and execute the proper api forwarding the json pack and compose
                 the response.
                 */
                virtual int handleCall(const std::vector<std::string>& api_tokens,
									   const Json::Value& input_data,
									   std::map<std::string, std::string>& output_header,
									   Json::Value& output_data) = 0;

            };
            
        }
    }
}
#endif
