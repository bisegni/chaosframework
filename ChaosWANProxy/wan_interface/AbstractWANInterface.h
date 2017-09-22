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
#ifndef __CHAOSFramework__AbstractWANInterface__
#define __CHAOSFramework__AbstractWANInterface__


#include "BaseWANInterfaceHandler.h"

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/UrlAccessibleService.h>

#include <json/json.h>

namespace chaos {
	namespace wan_proxy {
		//forward declaration
		class ChaosWANProxy;
		
		namespace wan_interface {
			
			class AbstractWANInterface:
			public common::utility::NamedService,
			public common::utility::StartableService,
			public common::utility::UrlAccessibleService {
				friend class chaos::wan_proxy::ChaosWANProxy;
                Json::Value						wan_interface_parameter;
                Json::Reader					json_reader;
			protected:
				int service_port;
				
				std::string service_url;
				
				AbstractWANInterface(const std::string& alias);
				
				~AbstractWANInterface();
				
				Json::Value& getParameter();
				
				BaseWANInterfacelHandler *handler;
			public:
				// inherited method
				//! init data need to be filled with json string
				void init(void *init_data) throw(chaos::CException);
				
				// inherited method
				void deinit() throw(chaos::CException);
				
				// inherited method
				void start() throw(chaos::CException);
				
				// inherited method
				void stop() throw(chaos::CException);
				
				//inherited method
				const std::string& getUrl();
				
				void setHandler(BaseWANInterfacelHandler *_handler);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__AbstractWANInterface__) */
