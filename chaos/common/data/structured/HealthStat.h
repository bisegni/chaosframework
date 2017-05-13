/*
 *	HealthStat.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/10/2016 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__9F37844_1F3C_4AE7_9CFA_D78611B3CC3B_HealthStat_h
#define __CHAOSFramework__9F37844_1F3C_4AE7_9CFA_D78611B3CC3B_HealthStat_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>

namespace chaos {
    namespace common {
        namespace data {
            namespace structured {
                //! define node overall status information
                struct HealthStat {
                    std::string node_uid;
                    uint64_t timestamp;
                    std::string health_status;
                    uint64_t uptime;
                    double user_time;
                    double sys_time;
                    
                    HealthStat():
                    node_uid(),
                    timestamp(0),
                    health_status(),
                    uptime(0),
                    user_time(0.0),
                    sys_time(0.0){}
                    
                    HealthStat(const HealthStat& health_stat_src):
                    node_uid(health_stat_src.node_uid),
                    timestamp(health_stat_src.timestamp),
                    health_status(health_stat_src.health_status),
                    uptime(health_stat_src.uptime),
                    user_time(health_stat_src.user_time),
                    sys_time(health_stat_src.sys_time){}
                };
                
                //! define serialization wrapper for dataset type
                CHAOS_OPEN_SDWRAPPER(HealthStat)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    Subclass::dataWrapped().node_uid = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, DataPackCommonKey::DPCK_DEVICE_ID, getStringValue, "");
                    Subclass::dataWrapped().timestamp = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP, getUInt64Value, 0);
                    Subclass::dataWrapped().uptime = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME, getUInt64Value, 0);
                    Subclass::dataWrapped().user_time = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, NodeHealtDefinitionKey::NODE_HEALT_USER_TIME, getDoubleValue, 0.0);
                    Subclass::dataWrapped().sys_time = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME,getDoubleValue, 0.0);
                    Subclass::dataWrapped().health_status = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, NodeHealtDefinitionKey::NODE_HEALT_STATUS, getStringValue, "");
                }
                
                std::unique_ptr<CDataWrapper> serialize() {
                    std::unique_ptr<CDataWrapper> result(new CDataWrapper());
                    result->addStringValue(DataPackCommonKey::DPCK_DEVICE_ID, Subclass::dataWrapped().node_uid);
                    result->addStringValue(NodeHealtDefinitionKey::NODE_HEALT_STATUS, Subclass::dataWrapped().health_status);
                    result->addInt64Value(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP, Subclass::dataWrapped().timestamp);
                    result->addInt64Value(NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME, Subclass::dataWrapped().uptime);
                    result->addDoubleValue(NodeHealtDefinitionKey::NODE_HEALT_USER_TIME, Subclass::dataWrapped().user_time);
                    result->addDoubleValue(NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME, Subclass::dataWrapped().sys_time);
                    return result;
                }
                CHAOS_CLOSE_SDWRAPPER()
            }
        }
    }
}

#endif /* __CHAOSFramework__9F37844_1F3C_4AE7_9CFA_D78611B3CC3B_HealthStat_h */
