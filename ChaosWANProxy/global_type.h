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
#ifndef CHAOSFramework_global_type_h
#define CHAOSFramework_global_type_h

#include <string>
#include <vector>

namespace chaos {
	namespace wan_proxy {
		
		typedef std::vector<std::string>			SettingStringList;
		typedef std::vector<std::string>::iterator	SettingStringListIterator;
		
		//! group of all setting of the wan proxy
		struct settings {
			//! list of the CDS server to use
			SettingStringList list_cds_server;
			
			//! a json block of parameter for all wan interfaces
			/*!
			 Each wan interface need to prefix the prameter name
			 with his name ex: HTTP_param_name, .... Attribute
			 without prefix are intend for global usage
			 */
			std::string parameter_wan_interfaces;
			
			//! list of the interface to activate
			SettingStringList list_wan_interface_to_enable;
		};
		
	}
}
#endif
