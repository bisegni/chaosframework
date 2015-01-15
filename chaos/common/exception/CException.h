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
        std::string msg;
        
        void composeMsg() {
            std::stringstream ss;
			ss << "-----------Exception------------"<<std::endl;
            ss << "Domain:" << errorDomain<<std::endl;
            ss << "Message:" << errorMessage<<std::endl;
            ss << "Error Code;" << errorCode<<std::endl;
            ss << "-----------Exception------------"<<std::endl;
            const std::string& strSS = ss.str();
            msg = strSS.c_str();
        }
    public:
        //! identify the number for the error
        int errorCode;
        //! describe the error that occour
        std::string errorMessage;
        //! identify the domain(ControlUnit, DataManager, ....)
        std::string errorDomain;

        
        explicit CException(int eCode, const char * eMessage,  const char * eDomain):errorCode(eCode),
        errorMessage( eMessage, strlen( eMessage )),
        errorDomain( eDomain, strlen( eDomain )) {composeMsg();};
        
        explicit CException(int eCode, std::string eMessage,  std::string eDomain):errorCode(eCode),
        errorMessage(eMessage),
        errorDomain(eDomain) {composeMsg();};
        
        virtual ~CException() throw() {};
        
        virtual const char* what() const throw() {
            return msg.c_str();
        }
    };
}
#endif
