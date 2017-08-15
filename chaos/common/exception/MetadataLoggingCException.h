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

#ifndef __CHAOSFramework__MetadataLoggingCException_h
#define __CHAOSFramework__MetadataLoggingCException_h

#include <chaos/common/exception/CException.h>
#include <boost/noncopyable.hpp>

namespace chaos {
    namespace common {
        namespace exception {
            
            //! Eception that will be logged in mds
            /*!
             this CException sublcass has the same data of his superclass
             the only feature is that it can be submitte to the metadata server
             */
            class MetadataLoggingCException:
            public chaos::CException {
                explicit MetadataLoggingCException(int eCode,
                                                   const char * eMessage,
                                                   const char * eDomain) throw();
            public:

                
                explicit MetadataLoggingCException(const std::string& log_source,
                                                   int error_code,
                                                   const std::string& error_message,
                                                   const std::string& error_domain) throw();
                
                MetadataLoggingCException(const MetadataLoggingCException& _exception) throw();
                
                ~MetadataLoggingCException() throw();
                
            };
            
        }
    }
}

#endif /* __CHAOSFramework__MetadataLoggingCException_h */
