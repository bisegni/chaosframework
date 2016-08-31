/*
 *	AttributeDatasetAction.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 31/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__7E3BDC9_9108_47EE_A610_D3AAF5F45CC4_AttributeDatasetAction_h
#define __CHAOSFramework__7E3BDC9_9108_47EE_A610_D3AAF5F45CC4_AttributeDatasetAction_h

#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <chaos/common/data/structured/AlterDatasetAttributeAction.h>
#include <chaos/cu_toolkit/data_manager/action/DataBrokerAction.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace action {
                
                DECLARE_CLASS_FACTORY(AttributeDatasetAction, DataBrokerAction) {
                public:
                    AttributeDatasetAction(const std::string& name);
                    ~AttributeDatasetAction();
                    void executeAction(const common::data::structured::AlterDatasetAttributeAction& attribute_dataset_action);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__7E3BDC9_9108_47EE_A610_D3AAF5F45CC4_AttributeDatasetAction_h */
