/*
 *	service_error.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 09/06/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_B06354BE_6F79_484B_8209_9BF76DE6FD16_service_error_h
#define __CHAOSFramework_B06354BE_6F79_484B_8209_9BF76DE6FD16_service_error_h

#include <chaos/common/chaos_errors.h>

namespace chaos {
    namespace service_common {
        
        //!all mds error mapping
        namespace ErrorMDSCode {
        static const int EC_MDS_NODE_BAD_NAME = -10000;
        static const int EC_MDS_NODE_BAD_PATH_EXTRACTION = -10001;
        static const int EC_MDS_PARENT_NODE_NOT_IN_DOMAIN = -10002;
        static const int EC_MDS_NODE_DUPLICATION = -10003;
        }

    }
}

#endif /* __CHAOSFramework_B06354BE_6F79_484B_8209_9BF76DE6FD16_service_error_h */
