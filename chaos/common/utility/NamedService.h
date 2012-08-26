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
    
        //! Named service utilityes class
    /*!
     This class is a base class for all service that need to be recognized with an alias
     */
    class NamedService {
        string *typeName;
    protected:
        NamedService(string *alias);
    public:
        /*
         Return the alias of the current instance
         \return the alias
         */
        const char * getName() const;
    };
}
#endif /* defined(__CHAOSFramework__NamedService__) */
