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

#pragma once

namespace bson {

    inline bool isNaN(double d) { 
        return d != d;
    }

    inline bool isInf(double d, int* sign = 0) {
        volatile double tmp = d;

        if ((tmp == d) && ((tmp - d) != 0.0)) {
            if ( sign ) {
                *sign = (d < 0.0 ? -1 : 1);
            }
            return true;
        }
        
        if ( sign ) {
            *sign = 0;
        }

        return false;
    }

}
