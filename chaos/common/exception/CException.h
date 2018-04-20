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
    std::ostream& operator<<(std::ostream &os,const chaos::CException& ex);
}

#endif
