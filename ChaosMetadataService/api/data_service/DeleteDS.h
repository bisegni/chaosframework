//
//  DeleteDS.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 24/04/15.
//  Copyright (c) 2015 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__DeleteDS__
#define __CHAOSFramework__DeleteDS__

#include "../AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace data_service {

                class DeleteDS:
                public AbstractApi {

                public:
                    DeleteDS();
                    ~DeleteDS();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data) throw(chaos::CException);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DeleteDS__) */
