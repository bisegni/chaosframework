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

#include <chaos/common/exception/CException.h>
#include <boost/exception/all.hpp>
using namespace chaos;

CException::CException(int eCode,
                       std::string eMessage,
                       std::string eDomain) throw():
errorCode(eCode),
errorMessage(eMessage),
errorDomain(eDomain) {
    composeMsg();
};

CException::CException(const CException& _exception) throw() :
errorCode(_exception.errorCode),
errorMessage(_exception.errorMessage),
errorDomain(_exception.errorDomain) {
    composeMsg();
};

CException::~CException() throw() {
    
};

const char* CException::what() const throw() {
    return msg.c_str();
}


void CException::composeMsg() {
    std::stringstream ss;
    ss << "-----------Exception------------"<<std::endl;
    ss << "Domain:" << errorDomain<<std::endl;
    ss << "Message:" << errorMessage<<std::endl;
    ss << "Error Code;" << errorCode<<std::endl;
    ss << "-----------Exception------------"<<std::endl;
    msg = ss.str();
}

const char* CFatalException::what() const throw() {
	std::stringstream ret;
	std::string stack;
	ret<<"FATAL:\""+msg<<"\"";
	stack =boost::diagnostic_information(*this);
	 if (stack.size() ) {
	        ret <<" stack:"<< stack;
	    }

    return ret.str().c_str();
}
