//
//  HLDataApi.h
//  ChaosFramework
//
//  Created by Claudio Bisegni on 02/10/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosFramework_HLDataApi_h
#define ChaosFramework_HLDataApi_h

#include <string>

namespace chaos {
    namespace ui {
        using namespace std;
        /*
         High level api for maning device datasert
         */
        class HLDataApi {
            friend class ChaosUIToolkit;
        
        /*
         * Constructor
         */
        HLDataApi();
        
        /*
         * Distructor
         */
        ~HLDataApi();

        public:

        };
    }
}



#endif
