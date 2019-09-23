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

#ifndef __CHAOSFramework__Testing_h
#define __CHAOSFramework__Testing_h

#include "test_types.h"

#include <chaos/common/chaos_types.h>

#include <vector>

namespace chaos {
namespace metadata_service {
namespace test {
class Testing {
    std::vector<ChaosSharedPtr<Test>> test_list;
public:
    Testing();
    ~Testing();
    
    void executeTest();
};
}
}
}

#endif /* __CHAOSFramework__Testing_h */
