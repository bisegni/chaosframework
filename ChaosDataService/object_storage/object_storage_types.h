/*
 *	obj_storage_types.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/09/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_FBC952DE_D3B4_440C_B6B4_4E64EB22DCFC_obj_storage_types_h
#define __CHAOSFramework_FBC952DE_D3B4_440C_B6B4_4E64EB22DCFC_obj_storage_types_h

#include <chaos/common/chaos_types.h>

#include <chaos/common/configuration/GlobalConfiguration.h>

namespace chaos {
    namespace data_service {
        namespace object_storage {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string,
                                      std::string,
                                      MapKVP);
            
#define OPT_OBJ_STORAGE_DRIVER                      "obj-storage-driver"
#define OPT_OBJ_STORAGE_SERVER_URL                  "obj-storage-driver-server_url"
#define OPT_OBJ_STORAGE_LOG_METRIC                  "obj-storage-driver-log-metric"
#define OPT_OBJ_STORAGE_LOG_METRIC_UPDATE_INTERVAL  "obj-storage-driver-log-metric-update-interval"
#define OPT_OBJ_STORAGE_DRIVER_KVP                  "obj-storage-kvp"
            
            //!
            struct ObjStorageSetting {
                std::string			driver_impl;
                ChaosStringVector   url_list;
                MapKVP              key_value_custom_param;
            };
        }
    }
}

#endif /* __CHAOSFramework_FBC952DE_D3B4_440C_B6B4_4E64EB22DCFC_obj_storage_types_h */
