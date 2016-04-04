/*
 *	node_monitor_types.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 16/03/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__node_monitor_types_h
#define __CHAOSFramework__node_monitor_types_h

#include <chaos/common/chaos_constants.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/CDataVariant.h>

namespace chaos {
    namespace metadata_service_client {
        namespace node_monitor {
            
            typedef enum {
                OnlineStatusNotFound,
                OnlineStatusUnknown,
                OnlineStatusON,
                OnlineStatusOFF
            } OnlineStatus;
            
            typedef struct ProcessResource{
                uint64_t            uptime;
                double              usr_res;
                double              sys_res;
                uint64_t            swp_res;
                ProcessResource& operator=(const ProcessResource& other) {
                    if (this == &other) return *this;
                    
                    uptime = other.uptime;
                    usr_res = other.usr_res;
                    sys_res = other.sys_res;
                    swp_res = other.swp_res;
                    return *this;
                }
            } ProcessResource;
            
            typedef struct ErrorInformation{
                int32_t         error_code;
                std::string     error_message;
                std::string     error_domain;
                
                ErrorInformation& operator=(const ErrorInformation& other) {
                    if (this == &other) return *this;
                    
                    error_code = other.error_code;
                    error_message = other.error_message;
                    error_domain = other.error_domain;
                    return *this;
                }
            } ErrorInformation;
            
            struct HealthInformation {
                OnlineStatus        online_status;
                ProcessResource     process_resource;
                ErrorInformation    error_information;
                
                HealthInformation& operator=(const HealthInformation& other) {
                    if (this == &other) return *this;
                    
                    online_status = other.online_status;
                    process_resource = other.process_resource;
                    error_information = other.error_information;
                    return *this;
                }
            };
            
            typedef enum {
                DatasetTypeOutput = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT,
                DatasetTypeInput = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT,
                DatasetTypeCustom = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM,
                DatasetTypeSystem = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM
            } DatasetType;
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, common::data::CDataVariant, MapDatasetKeyValues);
        }
    }
}

#endif /* __CHAOSFramework__node_monitor_types_h */
