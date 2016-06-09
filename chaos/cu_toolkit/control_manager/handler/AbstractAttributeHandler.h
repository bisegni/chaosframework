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
#include <chaos/common/data/DatasetDB.h>
#include <map>
#include <string>

#include <boost/lexical_cast.hpp>

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
                    //if present the max and min value are cheched befor launch handler
                    chaos::common::data::DatasetDB *dataset_database;
                    void setHandlerResult(const std::string& attribute_name,
                                          const bool result);
                    template<typename T>
                    void getRangeForKey(const std::string& key, T& min, T& max) {
                        if(dataset_database == NULL) return;
                        
                        chaos::common::data::RangeValueInfo range_info;
                        dataset_database->getAttributeRangeValueInfo(key,range_info);
                        
                        if(range_info.valueType == chaos::DataType::TYPE_INT32 ||
                           range_info.valueType == chaos::DataType::TYPE_INT64 ||
                           range_info.valueType == chaos::DataType::TYPE_DOUBLE) {
                            min = boost::lexical_cast<T>(range_info.minRange);
                            max = boost::lexical_cast<T>(range_info.maxRange);
                        }
                    }
                public:
                    AbstractAttributeHandler();
                    virtual ~AbstractAttributeHandler();
                    
                    virtual void executeHandlers(chaos::common::data::CDataWrapper *attribute_changes_set) = 0;
                    
                    bool getHandlerResult(const std::string& attirbute_name);
                    
                    void setDatasetDB(chaos::common::data::DatasetDB *ds_db_ptr);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__AbstractAttributeHandler_h */
