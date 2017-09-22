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
#ifndef CHAOSFramework_metadata_service_types_h
#define CHAOSFramework_metadata_service_types_h
#include <vector>
#include <string>
namespace chaos {
    namespace metadata_service_client {

        typedef std::vector<std::string>            BackendServerList;
        typedef std::vector<std::string>::iterator  BackendServerListIterator;
        typedef struct ClientSetting {
                //! is the list of the backend server to use for comunication
            BackendServerList mds_backend_servers;
        } ClientSetting;

    }
}

#endif
