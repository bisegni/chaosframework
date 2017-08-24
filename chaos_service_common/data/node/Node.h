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

#ifndef __CHAOSFramework__D60BF03_96AD_4396_A485_4ED793258A16_Node_h
#define __CHAOSFramework__D60BF03_96AD_4396_A485_4ED793258A16_Node_h

#include <chaos/common/data/TemplatedDataSDWrapper.h>
#include <chaos/common/chaos_constants.h>
namespace chaos {
    namespace service_common {
        namespace data {
            namespace node {
                
                //! base node instance description
                struct NodeInstance {
                    uint64_t    instance_seq;
                    std::string instance_name;
                    std::string rpc_addr;
                    std::string hostname;
                    
                    NodeInstance():
                    instance_seq(0),
                    instance_name(),
                    rpc_addr(),
                    hostname(){}
                    
                    NodeInstance(uint64_t _instance_seq,
                                 const std::string& _instance_name):
                    instance_seq(_instance_seq),
                    instance_name(_instance_name),
                    rpc_addr(),
                    hostname(){}
                    
                    NodeInstance(const NodeInstance& copy_src):
                    instance_seq(copy_src.instance_seq),
                    instance_name(copy_src.instance_name),
                    rpc_addr(copy_src.rpc_addr),
                    hostname(copy_src.hostname){}
                    
                    NodeInstance& operator=(NodeInstance const &rhs) {
                        instance_seq = rhs.instance_seq;
                        instance_name = rhs.instance_name;
                        rpc_addr = rhs.rpc_addr;
                        hostname = rhs.hostname;
                        return *this;
                    }
                };
                
                //sd wrapper for node instance class
                CHAOS_OPEN_SDWRAPPER(NodeInstance)
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                        if(serialized_data == NULL) return;
                        dataWrapped().instance_seq = (uint64_t)CDW_GET_INT64_WITH_DEFAULT(serialized_data, "instance_seq", 0);
                        dataWrapped().instance_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, "instance_name", "");
                        dataWrapped().rpc_addr = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeDefinitionKey::NODE_RPC_ADDR, "");
                        dataWrapped().hostname = CDW_GET_SRT_WITH_DEFAULT(serialized_data, NodeDefinitionKey::NODE_HOST_NAME, "");

                    }
                    
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                        ChaosUniquePtr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                        data_serialized->addInt64Value("instance_seq", dataWrapped().instance_seq);
                        data_serialized->addStringValue("instance_name", dataWrapped().instance_name);
                        data_serialized->addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, dataWrapped().rpc_addr);
                        data_serialized->addStringValue(NodeDefinitionKey::NODE_HOST_NAME, dataWrapped().hostname);
                        return data_serialized;
                    }
                CHAOS_CLOSE_SDWRAPPER()
            }
        }
    }
}

#endif /* __CHAOSFramework__D60BF03_96AD_4396_A485_4ED793258A16_Node_h */
