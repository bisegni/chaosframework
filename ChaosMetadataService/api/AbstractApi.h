//
//  AbstractApi.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 21/01/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#ifndef CHAOSFramework_AbstractApi_h
#define CHAOSFramework_AbstractApi_h
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/data/CDataWrapper.h>
namespace chaos {
	namespace metadata_service {
		namespace api {
			
			class AbstractApi:
			public chaos::common::utility::NamedService {
			public:
				AbstractApi(const std::string& name):NamedService(name){};
				~AbstractApi(){};
				virtual chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data, bool& detach_data) = 0;
			};
			
		}
	}
}

#endif
