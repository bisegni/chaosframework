//
//  ApiProvider.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 2/26/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_ApiProvider_h
#define CHAOSFramework_ApiProvider_h

#include "../data/DataManagment.h"
#include <chaos/common/action/DeclareAction.h>

namespace chaos {
     namespace cnd {
        namespace api {
            
            class ApiServer;
            
            //! ApiProvider
            /*!
             
             */
            class ApiProvider : public DeclareAction {
                friend class ApiServer;
            protected:
                cnd::data::DataManagment *dm;
            };
            
        }
    }
}

#endif
