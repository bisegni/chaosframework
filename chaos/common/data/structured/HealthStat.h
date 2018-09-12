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

#ifndef __CHAOSFramework__9F37844_1F3C_4AE7_9CFA_D78611B3CC3B_HealthStat_h
#define __CHAOSFramework__9F37844_1F3C_4AE7_9CFA_D78611B3CC3B_HealthStat_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/ProcStat.h>
#include <chaos/common/data/TemplatedDataSDWrapper.h>

namespace chaos {
    namespace common {
        namespace data {
            namespace structured {
                //! define node overall status information
                struct HealthStat:
                public chaos::common::utility::ProcStat {
                    std::string node_uid;
                    //!is the heath timesatmp of the node host
                    uint64_t timestamp;
                    //!is the timestamp of med at time that health dapapack has been received
                    int64_t mds_received_timestamp;
                    std::string health_status;
//                    uint64_t uptime;
//                    double user_time;
//                    double sys_time;
                    
                    HealthStat():
                    ProcStat(),
                    node_uid(),
                    timestamp(0),
                    health_status(){}
                    
                    HealthStat(const HealthStat& health_stat_src):
                    ProcStat(health_stat_src),
                    node_uid(health_stat_src.node_uid),
                    timestamp(health_stat_src.timestamp),
                    health_status(health_stat_src.health_status){}
                };
                
                //! define serialization wrapper for dataset type
                CHAOS_OPEN_SDWRAPPER(HealthStat)
                void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                    Subclass::dataWrapped().node_uid = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, DataPackCommonKey::DPCK_DEVICE_ID, getStringValue, "");
                    Subclass::dataWrapped().timestamp = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP, getUInt64Value, 0);
                    Subclass::dataWrapped().mds_received_timestamp = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, NodeHealtDefinitionKey::NODE_HEALT_MDS_TIMESTAMP, getUInt64Value, 0);
                    Subclass::dataWrapped().uptime = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME, getUInt64Value, 0);
                    Subclass::dataWrapped().usr_time = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, NodeHealtDefinitionKey::NODE_HEALT_USER_TIME, getDoubleValue, 0.0);
                    Subclass::dataWrapped().sys_time = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME,getDoubleValue, 0.0);
                    Subclass::dataWrapped().health_status = CDW_GET_VALUE_WITH_DEFAULT(serialized_data, NodeHealtDefinitionKey::NODE_HEALT_STATUS, getStringValue, "");
                }
                
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new CDataWrapper());
                    result->addStringValue(DataPackCommonKey::DPCK_DEVICE_ID, Subclass::dataWrapped().node_uid);
                    result->addStringValue(NodeHealtDefinitionKey::NODE_HEALT_STATUS, Subclass::dataWrapped().health_status);
                    result->addInt64Value(NodeHealtDefinitionKey::NODE_HEALT_TIMESTAMP, Subclass::dataWrapped().timestamp);
                    result->addInt64Value(NodeHealtDefinitionKey::NODE_HEALT_MDS_TIMESTAMP, Subclass::dataWrapped().mds_received_timestamp);
                    result->addInt64Value(NodeHealtDefinitionKey::NODE_HEALT_PROCESS_UPTIME, Subclass::dataWrapped().uptime);
                    result->addDoubleValue(NodeHealtDefinitionKey::NODE_HEALT_USER_TIME, Subclass::dataWrapped().usr_time);
                    result->addDoubleValue(NodeHealtDefinitionKey::NODE_HEALT_SYSTEM_TIME, Subclass::dataWrapped().sys_time);
                    return result;
                }
                CHAOS_CLOSE_SDWRAPPER()
            }
        }
    }
}

#endif /* __CHAOSFramework__9F37844_1F3C_4AE7_9CFA_D78611B3CC3B_HealthStat_h */
