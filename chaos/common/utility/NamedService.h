//
//  NamedService.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/08/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__NamedService__
#define __CHAOSFramework__NamedService__

#include <string>
namespace chaos {
    
    using namespace std;
    
    class NamedService {
        string *typeName;
    protected:
        NamedService(string *alias);
    public:
        /*
         Return the adapter alias
         */
        const char * getName() const;
    };
}
#endif /* defined(__CHAOSFramework__NamedService__) */
