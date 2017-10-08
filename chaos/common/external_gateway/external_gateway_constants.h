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

#ifndef __CHAOSFramework_F67CDB4D_86BB_49E0_9859_CE6BDCC3C989_external_gateway_constants_h
#define __CHAOSFramework_F67CDB4D_86BB_49E0_9859_CE6BDCC3C989_external_gateway_constants_h

namespace chaos{
    namespace common {
        namespace external_gateway {
            
#define CU_EG_OPT_WORKER_THREAD_NUMBER          "cu-eg-worker-thread-number"
#define CU_EG_OPT_WORKER_THREAD_NUMBER_DESC     "Specify the number ofr the thread that the external adapter work need to use"
#define CU_EG_OPT_WORKER_THREAD_NUMBER_DEFAULT  4

#define CU_EG_OPT_WORKER_KV_PARAM               "cu-eg-adapter-kv-param"
#define CU_EG_OPT_WORKER_KV_PARAM_DESC          "Abstraction key:value parameter used by external protocol adapter implementation"

        }
    }
}

#endif /* __CHAOSFramework_F67CDB4D_86BB_49E0_9859_CE6BDCC3C989_external_gateway_constants_h */
