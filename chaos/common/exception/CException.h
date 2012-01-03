//
//  ControlException.h
//  ControlSystemLib
//
//  Created by Claudio Bisegni on 13/03/11.
//  Copyright 2011 INFN. All rights reserved.
//

#ifndef ControlException_H
#define ControlException_H
#include <string>
using namespace std;
namespace chaos{
/*
 Base class for exception in control system library
 */
class CException {

public:
        //identify the number for the error
    int errorCode;
        //describe the error that occour
    string errorMessage;
        //identify the domain(ControlUnit, DataManager, ....)
    string errorDomain;

    CException(int eCode, const char * eMessage,  const char * eDomain){
        errorCode=eCode;
        if(eMessage)errorMessage = eMessage;
        if(eMessage)errorDomain = eDomain;
        
    };
};
}
#endif

