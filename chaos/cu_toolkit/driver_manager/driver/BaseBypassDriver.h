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
#ifndef __CHAOSFramework_ECA49652_F22D_45D5_A82E_7A29B3D410F6_BaseBypassDriver_h
#define __CHAOSFramework_ECA49652_F22D_45D5_A82E_7A29B3D410F6_BaseBypassDriver_h

#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>

namespace chaos{
    namespace cu {
        namespace driver_manager {
            namespace driver {
                //!base class a driver bypass
                class BaseBypassDriver:
                public OpcodeExecutor {
                protected:
                    MsgManagmentResultType::MsgManagmentResult execOpcode(DrvMsgPtr cmd);
                public:
                    BaseBypassDriver();
                    virtual ~BaseBypassDriver();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_ECA49652_F22D_45D5_A82E_7A29B3D410F6_AbstractBypassDriver_h */
