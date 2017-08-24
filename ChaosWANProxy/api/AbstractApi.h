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
#ifndef __CHAOSFramework__AbstractApi__
#define __CHAOSFramework__AbstractApi__

#include "api_types.h"
#include "PersistenceAccessor.h"

#include <string>
#include <vector>

#include <chaos/common/utility/NamedService.h>

#include <boost/thread/lockable_adapter.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <json/json.h>
namespace chaos {
	namespace wan_proxy {
		namespace api {
			
			//! abstract api class definition
			class AbstractApi:
			public PersistenceAccessor,
			public chaos::common::utility::NamedService,
			//! permit to the Api class to be used as mutex
			public boost::basic_lockable_adapter<boost::recursive_mutex> {
			protected:
				int setValueFromString(chaos::common::data::CDataWrapper& dataset,
									   const std::string& type,
									   const std::string& attribute_name,
									   const std::string& value);
			public:
				AbstractApi(const std::string& name,
							persistence::AbstractPersistenceDriver *_persistence_driver);
				virtual ~AbstractApi();
				
				virtual int execute(std::vector<std::string>& api_tokens,
									const Json::Value& input_data,
									std::map<std::string, std::string>& output_header,
									Json::Value& output_data) = 0;
			};
			
		}
	}
}

#endif /* defined(__CHAOSFramework__AbstractApi__) */
