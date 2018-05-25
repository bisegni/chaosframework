/*
 * Copyright 2012, 25/05/2018 INFN
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

#ifndef __CHAOSFramework__B2D2009_02A2_468B_9C5C_1D184ECB470F_ShardKeyManagement_h
#define __CHAOSFramework__B2D2009_02A2_468B_9C5C_1D184ECB470F_ShardKeyManagement_h

namespace chaos {
    namespace data_service {
        namespace object_storage {
            namespace mongodb {
                
                //!Class for the managment of the rotating shared key.
                /*!
                 
                 */
                class ShardKeyManagement {
                public:
                    ShardKeyManagement();
                    ~ShardKeyManagement();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__B2D2009_02A2_468B_9C5C_1D184ECB470F_ShardKeyManagement_h */
