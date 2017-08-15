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
