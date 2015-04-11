/*
 *	global_type.h
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
