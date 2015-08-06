/*
 *	PersistenceAccessor.h
 *	!CHAOS
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
#ifndef CHAOSFramework_PersistenceAccessor_h
#define CHAOSFramework_PersistenceAccessor_h

#include "../persistence/AbstractPersistenceDriver.h"
namespace chaos {
	namespace wan_proxy {
		namespace api {
			//! unify the access to the persistence driver
			class PersistenceAccessor {
				
			protected:
				//! persistence driver instance
				/*!
				 Thsi instance will be forward to all api that will use it
				 for make the work.
				 */
				persistence::AbstractPersistenceDriver *persistence_driver;
				
			public:
				PersistenceAccessor(persistence::AbstractPersistenceDriver *_persistence_driver);
				virtual ~PersistenceAccessor();
				
				persistence::AbstractPersistenceDriver *getPersistenceDriver();
			};
		}
	}
}

#endif
