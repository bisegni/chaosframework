/*
 *	FileCommandOutputStream.h
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

#ifndef __CHAOSFramework_CE042DA8_5C70_4683_BB17_CE36BFA51DA6_FileCommandOutputStream_h
#define __CHAOSFramework_CE042DA8_5C70_4683_BB17_CE36BFA51DA6_FileCommandOutputStream_h

#include "AbstractCommandOutputStream.h"

#include <iostream>

namespace chaos {
    namespace agent {
        namespace external_command_pipe {
            
            class FileCommandOutputStream:
            public AbstractCommandOutputStream {
                FILE *output_file;
            public:
                FileCommandOutputStream(const std::string& file_path);
                ~FileCommandOutputStream();
                void init(void *init_data) throw (chaos::CException);
                void deinit() throw (chaos::CException);
                virtual AbstractCommandOutputStream& operator<<(const std::string& string);
                virtual AbstractCommandOutputStream& operator<<(const char * string);
            };
            
        }
    }
}

#endif /* __CHAOSFramework_CE042DA8_5C70_4683_BB17_CE36BFA51DA6_FileCommandOutputStream_h */
