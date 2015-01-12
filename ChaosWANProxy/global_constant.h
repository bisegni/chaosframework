/*
 *	global_constant.h
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
#ifndef CHAOSFramework_global_constant_h
#define CHAOSFramework_global_constant_h

namespace chaos {
	namespace wan_proxy {
		//groups all setting option label
		namespace setting_options {
			//!  param for cds adressess
			static const char * const	OPT_CDS_ADDR   = "cds_addresses";
			
			//! specify the parameters in json (only string) for the wan interfaces
			static const char * const	OPT_WAN_INTERFACE_PARAM   = "wi_json_param";
			
			//! specify wich wan interface protocol to choice
			static const char * const	OPT_INTERFACE_TO_ACTIVATE   = "wi_interface";
		}
	}
}
#endif
