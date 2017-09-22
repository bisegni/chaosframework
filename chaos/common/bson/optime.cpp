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

#include "mongo/bson/optime.h"

#include <iostream>
#include <limits>
#include <ctime>

#include "mongo/bson/inline_decls.h"

namespace mongo {

    OpTime OpTime::max() {
        unsigned int t = static_cast<unsigned int>(std::numeric_limits<int32_t>::max());
        unsigned int i = std::numeric_limits<uint32_t>::max();
        return OpTime(t, i);
    }

}
