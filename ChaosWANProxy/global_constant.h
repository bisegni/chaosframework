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
#ifndef CHAOSFramework_global_constant_h
#define CHAOSFramework_global_constant_h

namespace chaos {
	namespace wan_proxy {
		//groups all setting option label
		namespace setting_options {
			//!  param for cds adressess
			static const char * const	OPT_CDS_ADDR   = "cds-addresses";
			
			//! specify the parameters in json (only string) for the wan interfaces
			static const char * const	OPT_WAN_INTERFACE_PARAM   = "wi-json-param";
			
			//! specify wich wan interface protocol to choice
			static const char * const	OPT_INTERFACE_TO_ACTIVATE   = "wi-interface";
		}
	}
}
#endif
