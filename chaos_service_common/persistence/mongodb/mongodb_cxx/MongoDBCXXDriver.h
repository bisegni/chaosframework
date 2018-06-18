/*
 * Copyright 2012, 18/06/2018 INFN
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

#ifndef _____PROJECTNAME_______UUIDASIDENTIFIER_______FILEBASENAMEASIDENTIFIER____h
#define _____PROJECTNAME_______UUIDASIDENTIFIER_______FILEBASENAMEASIDENTIFIER____h

#include <mongocxx/instance.hpp>

namespace chaos {
    namespace service_common {
        namespace persistence {
            namespace mongodb {
                namespace mongodb_cxx {
                    class BaseMongoDBDiver {
                        static mongocxx::instance drv_instance;
                    public:
                        BaseMongoDBDiver();
                        virtual ~BaseMongoDBDiver();
                    };
                }
            }
        }
    }
}

#endif /* _____PROJECTNAME_______UUIDASIDENTIFIER_______FILEBASENAMEASIDENTIFIER____h */
