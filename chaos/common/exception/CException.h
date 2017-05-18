/*
 *	ControlException.h
 *	!CHAOS
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
#include <cstring>
#include <exception>
#include <sstream>

#define MSG_ERROR_BUF 255

namespace chaos{
    /*
     Base class for exception in control system library
     */
    class CException : public std::exception {
        //! string stream for compose the "what" message
    	 void composeMsg();
    protected:
        mutable std::string msg;
        

    public:
        //! identify the number for the error
        const int errorCode;
        //! describe the error that occour
        const std::string errorMessage;
        //! identify the domain(ControlUnit, DataManager, ....)
        const std::string errorDomain;
        
        CException(int eCode, const std::string& eMessage,  const std::string& eDomain) throw();
        
        CException(const CException& _exception) throw();
        
        virtual ~CException() throw();
        
        virtual const char* what() const throw();
    };

    class CFatalException:public CException {
    	public:
    	 CFatalException(int eCode, const std::string& eMessage,  const std::string& eDomain);
    };
}
#endif
