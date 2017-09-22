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
