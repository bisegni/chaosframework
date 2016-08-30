/*
 *	DatabrokerUpdateDataset.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 30/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__AC9A8F3_E52A_4CE0_9858_FB7FA6579FC0_DatabrokerUpdateDataset_h
#define __CHAOSFramework__AC9A8F3_E52A_4CE0_9858_FB7FA6579FC0_DatabrokerUpdateDataset_h

#include <chaos/common/data/structured/Dataset.h>

namespace chaos {
    namespace common {
        namespace data {
            namespace structured {
                
                enum AlterDatasetAction {
                    AlterDatasetActionUpdate
                };
                
                //!describe the action that can be done on the dataset
                struct DatabrokerUpdateDataset {
                    
                    std::string dataset_name;
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__AC9A8F3_E52A_4CE0_9858_FB7FA6579FC0_DatabrokerUpdateDataset_h */
