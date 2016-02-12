/*
 *	MetadataLoggingexception.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 12/02/16 INFN, National Institute of Nuclear Physics
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
