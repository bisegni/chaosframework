/*
 *	chaos_errors.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 07/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/chaos_errors.h>
#include <chaos/common/exception/CException.h>

#include <limits.h>

using namespace chaos::error;

ChaosErrorCodeMapping::ChaosErrorCodeMapping() {}

void ChaosErrorCodeMapping::registerErrorDescription(int error_code,
                                                const std::string& error_message) {
    if(map_error_description.count(error_code)) throw CException(std::numeric_limits<int>::min(),
                                                                 "Error code already present",
                                                                 __PRETTY_FUNCTION__);
    //we can insert message
    map_error_description.insert(ChaosErrorMapPair(error_code, error_message));
}

const std::string& ChaosErrorCodeMapping::getErrorMessage(int error_code) {
    return map_error_description[error_code];
}
