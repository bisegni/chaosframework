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

#ifndef __CHAOSFramework__4D9D840_A775_4403_951A_2572E8F3AB65_external_unit_types_h
#define __CHAOSFramework__4D9D840_A775_4403_951A_2572E8F3AB65_external_unit_types_h

#include <chaos/common/external_unit/http_adapter/http_adapter_types.h>

namespace chaos{
    namespace common {
        namespace external_unit {
            //!define the message opcode used to manage the fragmentation of the data
            typedef enum {
                EUCMessageOpcodeWhole,          //! send in one shot all message data
                EUCPhaseStartFragment,          //!message is the beginning of the fragment
                EUCPhaseContinueFragment,       //!message is the cntinuation of the fragment
                EUCPhaseEndFragment             //!message is the end of the whole fragment
            } EUCMessageOpcode;
        }
    }
}

#endif /* __CHAOSFramework__4D9D840_A775_4403_951A_2572E8F3AB65_external_unit_types_h */
