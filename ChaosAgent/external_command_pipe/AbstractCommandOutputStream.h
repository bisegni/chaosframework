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

#ifndef __CHAOSFramework__0CAB9A0_3C50_400E_8C98_056013097EFB_AbstractCommandOutputStream_h
#define __CHAOSFramework__0CAB9A0_3C50_400E_8C98_056013097EFB_AbstractCommandOutputStream_h

#include <string>

#include <chaos/common/utility/InizializableService.h>


namespace chaos {
    namespace agent {
        namespace external_command_pipe {
            
            class AbstractCommandOutputStream:
            public chaos::common::utility::InizializableService {
            public:
                AbstractCommandOutputStream();
                virtual ~AbstractCommandOutputStream();
                
                void init(void *init_data) throw (chaos::CException);
                void deinit() throw (chaos::CException);
                
                virtual AbstractCommandOutputStream& operator<<(const std::string& string) = 0;
                virtual AbstractCommandOutputStream& operator<<(const char * string) = 0;
            };
        }
    }
}

#endif /* __CHAOSFramework__0CAB9A0_3C50_400E_8C98_056013097EFB_AbstractCommandOutputStream_h */
