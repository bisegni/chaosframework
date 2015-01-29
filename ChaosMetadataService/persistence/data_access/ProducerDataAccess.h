/*
 *	DeviceDataAccess.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyrigh 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__DeviceDataAccess__
#define __CHAOSFramework__DeviceDataAccess__

#include "../AbstractDataAccess.h"

#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
	namespace metadata_service {
		namespace persistence {
			namespace data_access {
				
				class ProducerDataAccess:
				public AbstractDataAccess {
				public:
                    //! default constructor
					ProducerDataAccess();
                    
                    //! defautl destructor
					~ProducerDataAccess();
                    
                    //! insert a new device with name and property
                    virtual int insertNewProducer(const std::string& producer_unique_name,
                                                  chaos::common::data::CDataWrapper& producer_property) = 0;
                    
                    //! check if a device is registered on persistence layer
                    virtual int checkProducerPresence(const std::string& producer_unique_name, bool& found) = 0;
                    
					//! save the new dataset of a producer
					virtual int saveDataset(const std::string& producer_unique_name,
                                            chaos::common::data::CDataWrapper& dataset_to_register) = 0;
					
                    //! return last dataaset of a producer
					virtual int loadLastDataset(const std::string& producer_unique_name,
												chaos::common::data::CDataWrapper **dataset_to_load) = 0;
					
					
				};
				
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__DeviceDataAccess__) */
