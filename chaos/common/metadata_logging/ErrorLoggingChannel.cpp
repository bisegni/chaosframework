/*
 *	ErrorLoggingChannel.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 10/02/16 INFN, National Institute of Nuclear Physics
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

#include "ErrorLoggingChannel.h"

using namespace chaos::common::data;
using namespace chaos::common::metadata_logging;

ErrorLoggingChannel::ErrorLoggingChannel():
AbstractMetadataLogChannel(){
    
}

ErrorLoggingChannel::~ErrorLoggingChannel() {
    
}

int ErrorLoggingChannel::logError(const chaos::CException& chaos_exception) {
    return logError(chaos_exception.errorCode,
                    chaos_exception.errorDomain,
                    chaos_exception.errorMessage);
}

int ErrorLoggingChannel::logError(int32_t error_code,
                                  const std::string& error_message,
                                  const std::string& error_domain) {
    CDataWrapper *log_entry = getNewLogEntry("error");
    return sendLog(log_entry,
                    true);
}