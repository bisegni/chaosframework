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
#ifndef ChaosFramework_LLDataApi_h
#define ChaosFramework_LLDataApi_h

#include <string>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/io/IODataDriver.h>
using namespace chaos::common::io;
using namespace std;
namespace chaos {
    namespace ui {
        /*
         Low level api for data access api
         */
        class LLDataApi:
		public common::utility::Singleton<LLDataApi>  {
            friend class ChaosUIToolkit;
            friend class Singleton<LLDataApi>;
            LLDataApi();
            ~LLDataApi();

          public:

        };
    }
}
#endif
