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
