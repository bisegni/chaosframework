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

#ifndef __CHAOSFramework_CE042DA8_5C70_4683_BB17_CE36BFA51DA6_FileCommandOutputStream_h
#define __CHAOSFramework_CE042DA8_5C70_4683_BB17_CE36BFA51DA6_FileCommandOutputStream_h

#include "AbstractCommandOutputStream.h"

#include <iostream>

namespace chaos {
    namespace agent {
        namespace external_command_pipe {
            
            class FileCommandOutputStream:
            public AbstractCommandOutputStream {
                int output_file;
                const std::string file_path;
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
