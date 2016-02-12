/*
 *	MetadataLoggingCException.cpp
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

#include <chaos/common/exception/MetadataLoggingCException.h>
#include <chaos/common/metadata_logging/metadata_logging.h>

using namespace chaos::common::exception;
using namespace chaos::common::metadata_logging;

MetadataLoggingCException::MetadataLoggingCException(int eCode, const char * eMessage,  const char * eDomain)throw():
CException(eCode, eMessage, eDomain) {}

MetadataLoggingCException::MetadataLoggingCException(const std::string& log_source,
                                                     int error_code,
                                                     const std::string& error_message,
                                                     const std::string& error_domain)throw():
CException(error_code,
           error_message,
           error_domain) {
    //get message channel
    ErrorLoggingChannel *log_chnl = (ErrorLoggingChannel*)MetadataLoggingManager::getInstance()->getChannel("ErrorLoggingChannel");
    if(log_chnl) {
        //log the error on mds
        log_chnl->logError(log_source,
                           error_code,
                           error_message,
                           error_domain);
        //release the channel
        MetadataLoggingManager::getInstance()->releaseChannel(log_chnl);
    }
}

MetadataLoggingCException::MetadataLoggingCException(const MetadataLoggingCException& _exception) throw():
CException(_exception){
    
}

MetadataLoggingCException::~MetadataLoggingCException() throw() {
    
}