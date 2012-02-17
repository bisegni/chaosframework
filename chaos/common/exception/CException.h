/*	
 *	ControlException.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *	
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */
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
