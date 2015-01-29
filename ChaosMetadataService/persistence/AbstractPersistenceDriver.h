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
#ifndef __CHAOSFramework__AbstractMSDatabaseDriver__
#define __CHAOSFramework__AbstractMSDatabaseDriver__

#include "AbstractDataAccess.h"
#include "data_access/DataAccess.h"

#include <string>

#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>

namespace chaos {
	namespace metadata_service{
		namespace persistence {
			
			//! Abstract base persistence driver
            /*!
             Define the rule for the persistence sublcass implementation.
             The scope of this class i instantiate the implementation of 
             the DataAccess base class
             */
			class AbstractPersistenceDriver:
			public common::utility::NamedService,
			public common::utility::InizializableService {
				
			public:
				AbstractPersistenceDriver(const std::string& name);
				virtual ~AbstractPersistenceDriver();
				
                //! return the implementation of the producer data access
				virtual data_access::ProducerDataAccess *getProducerDataAccess() = 0;
                
                //! return the implementation of the unit server data access
                virtual data_access::UnitServerDataAccess *getUnitServerDataAccess() = 0;
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__AbstractMSDatabaseDriver__) */
