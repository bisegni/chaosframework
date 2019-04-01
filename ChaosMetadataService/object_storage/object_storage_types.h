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

#ifndef __CHAOSFramework_FBC952DE_D3B4_440C_B6B4_4E64EB22DCFC_obj_storage_types_h
#define __CHAOSFramework_FBC952DE_D3B4_440C_B6B4_4E64EB22DCFC_obj_storage_types_h

#include <chaos/common/chaos_types.h>

#include <chaos/common/configuration/GlobalConfiguration.h>

namespace chaos {
    namespace metadata_service {
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
