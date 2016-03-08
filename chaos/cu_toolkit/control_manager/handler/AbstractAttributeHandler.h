/*
 *	AbstractAttributeHandler.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 08/03/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__AbstractAttributeHandler_h
#define __CHAOSFramework__AbstractAttributeHandler_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

#include <map>
#include <string>

namespace chaos {
    namespace cu {
        namespace control_manager {
            
            //! forward decalration
            class AbstractControlUnit;
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, bool, MapHandlerResult);
            
            namespace handler {
                //! the base class for the attribute's handler
                class AbstractAttributeHandler {
                    friend class AbstractControlUnit;
                protected:
                    MapHandlerResult map_handler_result;
                    
                    void setHandlerResult(const std::string& attribute_name,
                                          const bool result);
                public:
                    AbstractAttributeHandler();
                    virtual ~AbstractAttributeHandler();
                    
                    virtual void executeHandlers(chaos::common::data::CDataWrapper *attribute_changes_set) = 0;
                    
                    bool getHandlerResult(const std::string& attirbute_name);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__AbstractAttributeHandler_h */
