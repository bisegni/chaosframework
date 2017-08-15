/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/common/exception/CException.h>
#include <boost/exception/all.hpp>
using namespace chaos;

CException::CException(int eCode,
                       const std::string& eMessage,
                       const std::string& eDomain) throw():
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


CFatalException::CFatalException(int eCode,
                                 const std::string& eMessage,
                                 const std::string& eDomain):
CException(eCode,eMessage,eDomain) {
    std::stringstream ret;
    ret<<"FATAL:\""+msg<<"\"";
    msg = ret.str();
};
