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

#ifndef __CHAOSFramework__249F04F_EAE8_49F9_9509_0D0A5A72F0A1_DeployWorker_h
#define __CHAOSFramework__249F04F_EAE8_49F9_9509_0D0A5A72F0A1_DeployWorker_h

#include "../AbstractWorker.h"

#include <chaos_service_common/data/node/Agent.h>

namespace chaos {
    namespace agent {
        namespace worker {
            
            //! define the worker that permit to deploy chaos executable on host
            class DeployWorker:
            public AbstractWorker {
                
            protected:
                chaos::common::data::CDataWrapper *initDeploySession(chaos::common::data::CDataWrapper *data,
                                                                       bool& detach);
                
                chaos::common::data::CDataWrapper *uploadDeployChunk(chaos::common::data::CDataWrapper *data,
                                                                       bool& detach);
                
                chaos::common::data::CDataWrapper *endDeploySession(chaos::common::data::CDataWrapper *data,
                                                                         bool& detach);
            public:
                DeployWorker();
                ~DeployWorker();
                void init(void *data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
            };
        }
    }
}

#endif /* __CHAOSFramework__249F04F_EAE8_49F9_9509_0D0A5A72F0A1_DeployWorker_h */
