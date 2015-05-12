/*
 *	AbstractApi.h
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
#ifndef CHAOSFramework_AbstractApi_h
#define CHAOSFramework_AbstractApi_h
#include "../mds_types.h"
#include "../persistence/persistence.h"
#include "../batch/batch_impl.h"

#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/global.h>
namespace chaos {
	namespace metadata_service {
		namespace api {

#define CHECK_CDW_THROW_AND_LOG(cdw, log, num, msg)\
if(cdw == NULL) {LOG_AND_TROW(log, num, msg)}
            
#define CHECK_KEY_THROW_AND_LOG(cdw, key, log, num, msg)\
if(cdw->hasKey(key) == false) {LOG_AND_TROW(log, num, msg)}

#define GET_DATA_ACCESS(x,v, err)\
x *v = getPersistenceDriver()->getDataAccess<x>();\
if(v == NULL) throw CException(err, "Error allocating " #x, __PRETTY_FUNCTION__);

			//! Api abstraction
			/*!
			 This class define the rule for the api development
			 */
            class AbstractApi:
            public chaos::common::utility::NamedService,
            public chaos::common::utility::InizializableService {
                //! the instace of the persistence driver
                ApiSubserviceAccessor *subservice;
                
            protected:
                service_common::persistence::data_access::AbstractPersistenceDriver *getPersistenceDriver();
                batch::MDSBatchExecutor *getBatchExecutor();
                chaos::common::network::NetworkBroker *getNetworkBroker();
			public:
				//! defaukt constructor with the alias of the api
                AbstractApi(const std::string& name);
				
				//d! efault destructor
				virtual ~AbstractApi();
				
                void init(void *init_data) throw (chaos::CException);
                
                void deinit()  throw (chaos::CException);
                
				//! execute the api
				virtual chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                                   bool& detach_data) = 0;
			};
			
		}
	}
}

#endif
