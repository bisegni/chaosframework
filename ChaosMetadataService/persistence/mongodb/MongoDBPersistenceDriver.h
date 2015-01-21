/*
 *	MongoDBPersistenceDriver.h
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
#ifndef __CHAOSFramework__MongoDBPersistenceDriver__
#define __CHAOSFramework__MongoDBPersistenceDriver__

#include "../AbstractPersistenceDriver.h"
#include "MongoDBHAConnectionManager.h"

#include <chaos/common/utility/ObjectFactoryRegister.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
	namespace metadata_service {
		namespace persistence {
			namespace mongodb {
					
				//! mongodb implementation of persistence driver
				DECLARE_CLASS_FACTORY(MongoDBPersistenceDriver, AbstractPersistenceDriver) {
					REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(MongoDBPersistenceDriver)
					
					boost::shared_ptr<MongoDBHAConnectionManager> connection;
				protected:
					MongoDBPersistenceDriver(const std::string& name);
					~MongoDBPersistenceDriver();
					
				public:
					void init(void *init_data) throw (chaos::CException);
					void deinit() throw (chaos::CException);
					data_access::ProducerDataAccess *getProducerDataAccess();

				};
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__MongoDBPersistenceDriver__) */
