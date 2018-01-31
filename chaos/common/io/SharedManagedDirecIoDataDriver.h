/*
 * Copyright 2012, 31/01/2018 INFN
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

#ifndef __CHAOSFramework__CE8768D_5BB0_4EF2_A186_7685A0C31B3F_SharedManagedDirecIoDataDriver_h
#define __CHAOSFramework__CE8768D_5BB0_4EF2_A186_7685A0C31B3F_SharedManagedDirecIoDataDriver_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/io/ManagedDirectIODataDriver.h>


namespace chaos {
    namespace common {
        namespace io {
            
            //!shared class used by infrastrucutre for send data to data service layer
            class SharedManagedDirecIoDataDriver:
            public chaos::common::utility::InizializableService,
            public chaos::common::utility::Singleton<SharedManagedDirecIoDataDriver> {
                friend class chaos::common::utility::Singleton<SharedManagedDirecIoDataDriver>;
                ChaosSharedMutex init_mtx;
                ChaosSharedPtr<chaos::common::io::ManagedDirectIODataDriver> shared_data_driver;
            public:
                void init(void *data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
                const ChaosSharedPtr<chaos::common::io::ManagedDirectIODataDriver>& getSharedDriver();
            };
        }
    }
}

#endif /* __CHAOSFramework__CE8768D_5BB0_4EF2_A186_7685A0C31B3F_SharedManagedDirecIoDataDriver_h */
