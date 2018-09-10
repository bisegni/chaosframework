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

#ifndef __CHAOSFramework__B8D3722_2599_4337_999F_FADA42E4AC95_AbstractWorker_h
#define __CHAOSFramework__B8D3722_2599_4337_999F_FADA42E4AC95_AbstractWorker_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/utility/InizializableService.h>

#include <memory>

namespace chaos {
    namespace agent {
        
        class AbstractWorker:
        public DeclareAction,
        public chaos::common::utility::NamedService,
        public chaos::common::utility::InizializableService {
        public:
            AbstractWorker(const std::string& _worker_name);
            virtual ~AbstractWorker();
            
            void init(void *data) ;
            void deinit() ;
        };
    }
}

#endif /* __CHAOSFramework__B8D3722_2599_4337_999F_FADA42E4AC95_AbstractWorker_h */
