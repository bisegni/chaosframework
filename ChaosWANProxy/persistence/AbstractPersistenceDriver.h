/*
 *	AbstractPersistenceDriver.h
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
#ifndef __CHAOSFramework__AbstractPersistenceDriver__
#define __CHAOSFramework__AbstractPersistenceDriver__
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/NamedService.h>
namespace chaos {
	namespace wan_proxy {
		namespace persistence {
			
			//! define the base class fro all persistence implementation
			class AbstractPersistenceDriver:
			public chaos::common::utility::NamedService {
			public:
				AbstractPersistenceDriver(const std::string& name);
				virtual ~AbstractPersistenceDriver();
				
				//! push a dataset
                /*!
                 \param producer_key the producer key
                 \param new_dataset the dataset to store (the pointer remain owned by the creator)
                 \param hint for storage option
                 */
				virtual int pushNewDataset(const std::string& producer_key,
										   chaos::common::data::CDataWrapper *new_dataset,
										   int store_hint) = 0;
				
				//! get a dataset
				virtual int getLastDataset(const std::string& producer_key,
										   chaos::common::data::CDataWrapper **last_dataset) = 0;
				
				//! register the dataset of ap roducer
				virtual int registerDataset(const std::string& producer_key,
											chaos::common::data::CDataWrapper& last_dataset) = 0;
			};
		}
	}
}
#endif /* defined(__CHAOSFramework__AbstractPersistenceDriver__) */
