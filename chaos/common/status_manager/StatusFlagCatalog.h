/*
 *	StatusFlagCatalog.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 12/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__2D01FFA_30CE_47E9_9118_E53040270DCF_StatusFlagCatalog_h
#define __CHAOSFramework__2D01FFA_30CE_47E9_9118_E53040270DCF_StatusFlagCatalog_h

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataBuffer.h>
#include <chaos/common/status_manager/StatusFlag.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace status_manager {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(unsigned int, StatusFlag, MapSFCatalog);
            
            //! define a set of status flag with usefull operation on it
            class StatusFlagCatalog {
                const std::string catalog_name;
                //define the catalog where is assigned an unique id to a flag
                MapSFCatalog        catalog;
                mutable boost::shared_mutex mutex_catalog;
            public:
                StatusFlagCatalog(const std::string& _catalog_name);
                virtual ~StatusFlagCatalog();
                
                void addFlag(const StatusFlag& flag);
                void appendCatalog(const StatusFlagCatalog& src);
                
                std::auto_ptr<chaos::common::data::CDataBuffer> getRawFlagsLevel();
                
                void setApplyRawFlagsValue(std::auto_ptr<chaos::common::data::CDataBuffer> raw_level);
            };
            
        }
    }
}

#endif /* __CHAOSFramework__2D01FFA_30CE_47E9_9118_E53040270DCF_StatusFlagCatalog_h */
