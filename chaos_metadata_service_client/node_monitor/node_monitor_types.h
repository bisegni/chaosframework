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
                OnlineStateNotFound,
                OnlineStateUnknown,
                OnlineStateON,
                OnlineStateOFF
            } OnlineState;
            
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
                OnlineState         online_state;
                std::string         internal_state;
                ProcessResource     process_resource;
                ErrorInformation    error_information;
                
                HealthInformation& operator=(const HealthInformation& other) {
                    if (this == &other) return *this;
                    
                    online_state = other.online_state;
                    internal_state = other.internal_state;
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
            
            typedef enum {
                ControllerTypeNode = 0,
                ControllerTypeNodeControlUnit
            } ControllerType;
        }
    }
}

#endif /* __CHAOSFramework__node_monitor_types_h */
