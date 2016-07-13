/*
 *	Node.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 16/06/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__D60BF03_96AD_4396_A485_4ED793258A16_Node_h
#define __CHAOSFramework__D60BF03_96AD_4396_A485_4ED793258A16_Node_h

#include <chaos/common/data/TemplatedDataSDWrapper.h>

namespace chaos {
    namespace service_common {
        namespace data {
            namespace node {
                //! base node instance description
                struct NodeInstance {
                    uint64_t    instance_seq;
                    std::string instance_name;
                    
                    NodeInstance():
                    instance_seq(0),
                    instance_name(){}
                    
                    NodeInstance(uint64_t _instance_seq,
                                 const std::string& _instance_name):
                    instance_seq(_instance_seq),
                    instance_name(_instance_name){}
                    
                    NodeInstance(const NodeInstance& copy_src):
                    instance_seq(copy_src.instance_seq),
                    instance_name(copy_src.instance_name){}
                    
                    NodeInstance& operator=(NodeInstance const &rhs) {
                        instance_seq = rhs.instance_seq;
                        instance_name = rhs.instance_name;
                        return *this;
                    }
                };
                
                //sd wrapper for node instance class
                CHAOS_DEFINE_TEMPLATED_DATA_SDWRAPPER_CLASS(NodeInstance) {
                public:
                    NodeInstanceSDWrapper():
                    NodeInstanceSDWrapperSubclass(){}
                    
                    NodeInstanceSDWrapper(const NodeInstance& copy_source):
                    NodeInstanceSDWrapperSubclass(copy_source){}
                    
                    NodeInstanceSDWrapper(common::data::CDataWrapper *serialized_data):
                    NodeInstanceSDWrapperSubclass(serialized_data){
                        deserialize(serialized_data);
                    }
                    
                    
                    void deserialize(chaos::common::data::CDataWrapper *serialized_data) {
                        if(serialized_data == NULL) return;
                        dataWrapped().instance_seq = (uint64_t)CDW_GET_INT64_WITH_DEFAULT(serialized_data, "instance_seq", 0);
                        dataWrapped().instance_name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, "instance_name", "");
                    }
                    
                    std::auto_ptr<chaos::common::data::CDataWrapper> serialize(const uint64_t sequence = 0) {
                        std::auto_ptr<chaos::common::data::CDataWrapper> data_serialized(new chaos::common::data::CDataWrapper());
                        data_serialized->addInt64Value("instance_seq", (sequence?sequence:dataWrapped().instance_seq));
                        data_serialized->addStringValue("instance_name", dataWrapped().instance_name);
                        return data_serialized;
                    }
                };

            }
        }
    }
}

#endif /* __CHAOSFramework__D60BF03_96AD_4396_A485_4ED793258A16_Node_h */
