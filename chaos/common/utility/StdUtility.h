//
//  StdUtility.h
//  ChaosLib
//
//  Created by Bisegni Claudio on 06/11/11.
//  Copyright (c) 2011 INFN. All rights reserved.
//

#ifndef ChaosLib_StdUtility_h
#define ChaosLib_StdUtility_h

namespace cs {
    struct CharStrPointerCompare {
        bool operator()(char const *a, char const *b) {
            return std::strcmp(a, b) < 0;
        }
    };
}

#endif
