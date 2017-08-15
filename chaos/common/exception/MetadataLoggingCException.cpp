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

#include <chaos/common/exception/MetadataLoggingCException.h>
#include <chaos/common/metadata_logging/metadata_logging.h>

using namespace chaos::common::exception;
using namespace chaos::common::metadata_logging;

MetadataLoggingCException::MetadataLoggingCException(int eCode,
                                                     const char * eMessage,
                                                     const char * eDomain)throw():
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
                           "MetadataLoggingCException",
                           error_code,
                           error_message,
                           error_domain);
        //release the channel
        MetadataLoggingManager::getInstance()->releaseChannel(log_chnl);
    }
}

MetadataLoggingCException::MetadataLoggingCException(const MetadataLoggingCException& _exception) throw():
CException(_exception){}

MetadataLoggingCException::~MetadataLoggingCException() throw() {
    
}
