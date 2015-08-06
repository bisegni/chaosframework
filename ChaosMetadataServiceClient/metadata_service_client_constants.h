/*
 *	metadata_service_client_constants.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef CHAOSFramework_metadata_service_client_constants_h
#define CHAOSFramework_metadata_service_client_constants_h

namespace chaos {
    namespace metadata_service_client {

            //! Description for the control unit dirvers [vector[string, string, string]*]
        static const char * const OPT_MDS_ADDRESS                = "mds-address";
        static const char * const OPT_MDS_ADDRESS_DESCRIPTION    = "The list of the metadata servers address(ip:port) to use as backend";

    }
}
#endif
