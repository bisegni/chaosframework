    //
    //  DataManagerException.h
    //  ControlSystemLib
    //
    //  Created by Claudio Bisegni on 30/04/11.
    //  Copyright 2011 INFN. All rights reserved.
    //
#ifndef DataManagerException_H
#define DataManagerException_H

#include <chaos/common/exception/CException.h>

namespace chaos{
    /*
     Exception class for DataManager package
     */
    class DataManagerException : public CException{
        
        
    public:
        DataManagerException(int eCode, const char * eMessage):ControlException(eCode, eMessage, "DataManager") {
            
        }
    };
}
#endif