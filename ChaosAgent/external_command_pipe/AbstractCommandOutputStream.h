/*
 *	AbstractCommandOutputStream.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/03/2017 INFN, National Institute of Nuclear Physics
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
